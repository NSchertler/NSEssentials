/*
	This file is part NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#ifdef HAVE_EIGEN
#include <Eigen/Sparse>
#include "nsessentials/data/Parallelization.h"

namespace nse {
	namespace math
	{
		template <int SolutionColumns, typename Scalar = float>
		class LeastSquaresSystem;

		// Represents a row in the linear system A x = b
		template <int SolutionColumns, typename Scalar = float>
		class LinearSystemRow
		{
		public:
			//Resets the row to zero
			void reset()
			{
				lhsCoefficients.clear();
				rhs.setZero();
			}

			//Adds the summand coefficient * x_index to the left-hand side of the equation.
			void addCoefficient(int index, Scalar coefficient)
			{
				lhsCoefficients.push_back(std::make_pair(index, coefficient));
			}

			//Adds the summand coefficient * fixed to the left-hand side of the equation.
			void addCoefficientWithFixed(Scalar coefficient, const Eigen::Matrix<Scalar, SolutionColumns, 1>& fixed)
			{
				rhs -= coefficient * fixed;
			}

			//Adds value to the right-hand side of the equation.
			void addToRHS(const Eigen::Matrix<Scalar, SolutionColumns, 1>& value)
			{
				rhs += value;
			}

			template <typename ReturnType = typename std::enable_if<SolutionColumns == 1, void>::type>
			ReturnType addToRHS(Scalar value)
			{
				rhs(0) += value;
			}


		private:
			std::vector<std::pair<int, Scalar>> lhsCoefficients;
			Eigen::Matrix<Scalar, SolutionColumns, 1> rhs;

			template <int N, typename TScalar>
			friend class LeastSquaresSystem;
		};

		// Represents a sparse linear least squares system as normal equations A^T A x = A^T b.
		// SolutionColumns is the number of columns of x and b.
		template <int SolutionColumns, typename Scalar>
		class LeastSquaresSystem
		{
		public:
			typedef Eigen::SparseMatrix<Scalar> MatrixType;

			LeastSquaresSystem() { }
			LeastSquaresSystem(int numberOfUnknowns)
				: lhs(numberOfUnknowns, numberOfUnknowns), rhs(Eigen::Matrix<Scalar, -1, SolutionColumns>::Zero(numberOfUnknowns, SolutionColumns))
			{
				rhs.setZero();
			}

			// Updates the system in a way that is equivalent to adding another row in A and b in A x = b.
			// The first SkipColumns columns of the row's right-hand side are ignored.
			template <int SkipColumns, int Columns>
			void addRow(const LinearSystemRow<Columns, Scalar>& row, Scalar weight = 1)
			{
				for (auto& entry1 : row.lhsCoefficients)
				{
					for (auto& entry2 : row.lhsCoefficients)
					{
						lhs.coeffRef(entry1.first, entry2.first) += weight * entry1.second * entry2.second;
					}
					rhs.row(entry1.first) += weight * entry1.second * row.rhs.template block<SolutionColumns, 1>(SkipColumns, 0).transpose();
				}
			}

			// Updates the system in a way that is equivalent to adding another row in A and b in A x = b.
			// row is a list of index/value pairs. solution is the right-hand side of the equation.
			void addRow(const LinearSystemRow<SolutionColumns, Scalar>& row, Scalar weight = 1)
			{
				addRow<0>(row, weight);
			}

			// Has the same semantics as addRow but performs all operations atomically. This requires
			// that all entries in the matrix are already present.
			template <int SkipColumns, int Columns>
			void addRowAtomic(const LinearSystemRow<Columns, Scalar>& row, Scalar weight = 1)
			{
				for (auto& entry1 : row.lhsCoefficients)
				{
					if (entry1.second == 0)
						continue;
					for (auto& entry2 : row.lhsCoefficients)
					{
						if (entry2.second == 0)
							continue;
						Scalar* coeff = &lhs.coeffRef(entry1.first, entry2.first);
						nse::data::atomicAdd(coeff, weight * entry1.second * entry2.second);
					}
					rhs.row(entry1.first) += weight * entry1.second * row.rhs.template block<SolutionColumns, 1>(SkipColumns, 0).transpose();
				}
			}

			// Has the same semantics as addRow but performs all operations atomically. This requires
			// that all entries in the matrix are already present.
			void addRowAtomic(const LinearSystemRow<SolutionColumns, Scalar>& row, Scalar weight = 1)
			{
				addRowAtomic<0>(row, weight);
			}

			template <typename EigenSolver>
			Eigen::Matrix<Scalar, Eigen::Dynamic, SolutionColumns> solve(EigenSolver& solver, const Eigen::Matrix<Scalar, Eigen::Dynamic, SolutionColumns>& initialGuess)
			{
				solver.compute(lhs);

				Eigen::Matrix<Scalar, Eigen::Dynamic, SolutionColumns> solution(initialGuess.rows(), SolutionColumns);
				solver.solveWithGuess(rhs, initialGuess, solution);
				return solution;
			}

			// This overload can be used when you want to solve only a subset of available columns of the solution.
			template <int TotalColumnsOfGuessAndSolution, typename EigenSolver>
			void solve(EigenSolver& solver, const Eigen::Matrix<Scalar, Eigen::Dynamic, TotalColumnsOfGuessAndSolution>& initialGuess, Eigen::Matrix<Scalar, Eigen::Dynamic, TotalColumnsOfGuessAndSolution>& solution)
			{
				solver.compute(lhs);

				solver.solveWithGuess(rhs, initialGuess, solution);
			}

			size_t numberOfUnknowns() const { return lhs.rows(); }

		public:
			MatrixType lhs;
			Eigen::Matrix<Scalar, -1, SolutionColumns> rhs;
		};
	}
}
#endif