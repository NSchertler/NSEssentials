/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <nsessentials/math/QuadraticEnergy.h>
#include <list>

// Solves a quadratic mixed integer problem by greedy rounding as published in "Mixed-Integer Quadrangulation".
// The algorithm iteratively rounds the integer variables on by one followed by an update of the continuous part.
// The continuous part is first updated with local Gauss-Seidel iterations. If this fails in the given number of
// iterations, a custom iterative solver is used.
// The energy will be changed as integer variables are fixed and will only have the continuous variables unfixed
// after calling this function.
template <typename Scalar, typename TIterativeSolver>
void solveMixedIntegerGreedyRounding(
	nse::math::QuadraticEnergy<1, Scalar>& energy, const std::vector<int> integers, 
	Eigen::Matrix<Scalar, -1, 1>& solution,
	TIterativeSolver& iterativeSolver, 
	const int maxGaussSeidelIterations = 100, const Scalar gaussSeidelTolerance = 1e-6)
{
	std::list<int> unfixedIntegers(integers.begin(), integers.end());

	std::vector<std::pair<int, Scalar>> change;
	change.emplace_back(0, 0);

	Scalar residualThreshold = energy.b().norm() * gaussSeidelTolerance;

	auto enqueueNonZeros = [&](int variableIdx, std::queue<int>& queue)
	{
		for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it(energy.A(), variableIdx); it; ++it)
		{
			if (it.index() != variableIdx && it.value() != 0 && !energy.isVariableFixed(it.index()))
				queue.push(it.index());
		}
	};
	
	{
		while (!unfixedIntegers.empty())
		{
			//find the best integer to round
			std::list<int>::iterator best = unfixedIntegers.end();
			Scalar bestRoundError = std::numeric_limits<Scalar>::infinity();
			Scalar newValue;
			for (auto it = unfixedIntegers.begin(); it != unfixedIntegers.end(); ++it)
			{
				auto currentValue = solution(*it);
				auto rounded = std::round(currentValue);
				change[0].first = *it;
				change[0].second = rounded;
				auto energyChange = energy.energyChange(solution, change);
				if (energyChange < bestRoundError)
				{
					bestRoundError = energyChange;
					best = it;
					newValue = rounded;
				}
			}

			if (best == unfixedIntegers.end())
			{
				std::cout << "Error while finding the least round-off error." << std::endl;
				return;
			}

			//update variable and perform local Gauss-Seidel
			solution(*best) = newValue;
			std::queue<int> affectedVariables;
			enqueueNonZeros(*best, affectedVariables);
			energy.fixVariable(*best, newValue);
			unfixedIntegers.erase(best);

			int iter = 0;
			while (!affectedVariables.empty() && iter < maxGaussSeidelIterations)
			{
				++iter;
				int var = affectedVariables.front();
				affectedVariables.pop();

				//update residual
				Scalar residual = energy.b()(var);
				for (typename Eigen::SparseMatrix<Scalar>::InnerIterator it(energy.A(), var); it; ++it)
					residual -= it.value() * solution(it.index());

				//check if we need to update this variable
				if (std::abs(residual) > residualThreshold)
				{
					auto diagCoeff = energy.A().coeff(var, var);
					if (diagCoeff == 0)
					{
						//matrix is not diagonally dominant here, go directly to iterative solver
						iter = maxGaussSeidelIterations;
						break;
					}
					solution(var) += residual / diagCoeff;
					enqueueNonZeros(var, affectedVariables);
				}
			}

			if (iter == maxGaussSeidelIterations)			
				iterativeSolver.solveWithGuess(energy.b(), solution, solution);
		}
	}
}