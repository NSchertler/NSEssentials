/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#ifdef HAVE_EIGEN
#include "nsessentials/math/LeastSquaresSystem.h"
#include "nsessentials/data/Parallelization.h"

#include <iostream>

using namespace nse::math;

template<int SolutionColumns>
inline LeastSquaresSystem<SolutionColumns>::LeastSquaresSystem()
{
}

template<int SolutionColumns>
inline LeastSquaresSystem<SolutionColumns>::LeastSquaresSystem(int numberOfUnknowns)
	: lhs(numberOfUnknowns, numberOfUnknowns), rhs(Eigen::Matrix<float, -1, SolutionColumns>::Zero(numberOfUnknowns, SolutionColumns))
{
	rhs.setZero();
}

template <int SolutionColumns>
void LeastSquaresSystem<SolutionColumns>::addRow(const LinearSystemRow<SolutionColumns>& row, float weight)
{
	addRow<0>(row, weight);
}

template <int SolutionColumns>
void LeastSquaresSystem<SolutionColumns>::addRowAtomic(const LinearSystemRow<SolutionColumns>& row, float weight)
{
	addRowAtomic<0>(row, weight);
}

template<int SolutionColumns>
void LinearSystemRow<SolutionColumns>::reset()
{
	lhsCoefficients.clear();
	rhs.setZero();
}

template<int SolutionColumns>
void LinearSystemRow<SolutionColumns>::addCoefficient(int index, float coefficient)
{
	lhsCoefficients.push_back(std::make_pair(index, coefficient));
}

template<int SolutionColumns>
void LinearSystemRow<SolutionColumns>::addCoefficientWithFixed(float coefficient, const Eigen::Matrix<float, SolutionColumns, 1>& fixed)
{
	rhs -= coefficient * fixed;
}

template<int SolutionColumns>
void LinearSystemRow<SolutionColumns>::addToRHS(const Eigen::Matrix<float, SolutionColumns, 1>& value)
{
	rhs += value;
}

namespace nse
{
	namespace math
	{
		template class LinearSystemRow<1>;
		template class LeastSquaresSystem<1>;
		template class LinearSystemRow<2>;
		template class LeastSquaresSystem<2>;
		template class LinearSystemRow<3>;
		template class LeastSquaresSystem<3>;
		template class LinearSystemRow<4>;
		template class LeastSquaresSystem<4>;
	}
}
#endif