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

template<int SolutionColumns, typename Scalar>
inline LeastSquaresSystem<SolutionColumns, Scalar>::LeastSquaresSystem()
{
}

template<int SolutionColumns, typename Scalar>
inline LeastSquaresSystem<SolutionColumns, Scalar>::LeastSquaresSystem(int numberOfUnknowns)
	: lhs(numberOfUnknowns, numberOfUnknowns), rhs(Eigen::Matrix<Scalar, -1, SolutionColumns>::Zero(numberOfUnknowns, SolutionColumns))
{
	rhs.setZero();
}

template <int SolutionColumns, typename Scalar>
void LeastSquaresSystem<SolutionColumns, Scalar>::addRow(const LinearSystemRow<SolutionColumns, Scalar>& row, Scalar weight)
{
	addRow<0>(row, weight);
}

template <int SolutionColumns, typename Scalar>
void LeastSquaresSystem<SolutionColumns, Scalar>::addRowAtomic(const LinearSystemRow<SolutionColumns, Scalar>& row, Scalar weight)
{
	addRowAtomic<0>(row, weight);
}

template<int SolutionColumns, typename Scalar>
void LinearSystemRow<SolutionColumns, Scalar>::reset()
{
	lhsCoefficients.clear();
	rhs.setZero();
}

template<int SolutionColumns, typename Scalar>
void LinearSystemRow<SolutionColumns, Scalar>::addCoefficient(int index, Scalar coefficient)
{
	lhsCoefficients.push_back(std::make_pair(index, coefficient));
}

template<int SolutionColumns, typename Scalar>
void LinearSystemRow<SolutionColumns, Scalar>::addCoefficientWithFixed(Scalar coefficient, const Eigen::Matrix<Scalar, SolutionColumns, 1>& fixed)
{
	rhs -= coefficient * fixed;
}

template<int SolutionColumns, typename Scalar>
void LinearSystemRow<SolutionColumns, Scalar>::addToRHS(const Eigen::Matrix<Scalar, SolutionColumns, 1>& value)
{
	rhs += value;
}

template <int SolutionColumns, typename Scalar>
template <typename ReturnType>
ReturnType LinearSystemRow<SolutionColumns, Scalar>::addToRHS(Scalar value)
{
	rhs(0) += value;
}
#endif