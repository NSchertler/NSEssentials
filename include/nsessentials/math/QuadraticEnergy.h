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
		template <int SubEnergie = 1, typename Scalar = float>
		class QuadraticEnergy;
		
		// Represents a linear constraint of the form:
		//   Σ c_i * x_i = b
		template <int SubEnergies = 1, typename Scalar = float>
		class LinearConstraint
		{
		public:
			LinearConstraint()
			{
				reset();
			}

			//Resets the term to zero
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
			void addCoefficientWithFixed(Scalar coefficient, const Eigen::Matrix<Scalar, SubEnergies, 1>& fixed)
			{
				rhs -= coefficient * fixed;
			}

			//Adds the summand coefficient * fixed to the left-hand side of the equation.
			template <int Cols = SubEnergies>
			void addCoefficientWithFixed(Scalar coefficient, typename std::enable_if<Cols == 1, Scalar>::type fixed)
			{
				rhs(0) -= coefficient * fixed;
			}

			//Adds value to the right-hand side of the equation.
			void addToRHS(const Eigen::Matrix<Scalar, SubEnergies, 1>& value)
			{
				rhs += value;
			}

			template <int Cols = SubEnergies>
			void addToRHS(typename std::enable_if<Cols == 1, Scalar>::type value)
			{
				rhs(0) += value;
			}

			const std::vector<std::pair<int, Scalar>>& coefficientsLHS() const { return lhsCoefficients; }
			const Eigen::Matrix<Scalar, SubEnergies, 1>& RHS() const { return rhs; }

		private:
			std::vector<std::pair<int, Scalar>> lhsCoefficients;
			Eigen::Matrix<Scalar, SubEnergies, 1> rhs;

			template <int N, typename TScalar>
			friend class QuadraticEnergy;
		};

		// Represents several quadratic energies in the form
		//   E(x) = x^T A x - 2 x^T b + c
		// All energies share the same A but can have different b and c
		// SubEnergies is the number of columns of x, b, and c.
		template <int SubEnergies, typename Scalar>
		class QuadraticEnergy
		{
		public:
			typedef Eigen::SparseMatrix<Scalar> MatrixType;

			QuadraticEnergy() { }
			QuadraticEnergy(int numberOfUnknowns, int linearConstraints = 0)
				:
				_A(numberOfUnknowns + linearConstraints, numberOfUnknowns + linearConstraints),
				_b(Eigen::Matrix<Scalar, -1, SubEnergies>::Zero(numberOfUnknowns + linearConstraints, SubEnergies)),
				_c(Eigen::Matrix<Scalar, 1, SubEnergies>::Zero(1, SubEnergies)),
				unknowns(numberOfUnknowns), nextConstraint(numberOfUnknowns)
			{ }

			// Calculates a specific subenergy for a given argument x.
			//   iSubEnergy: the index of the subenergy to calculate, must be in [0, SubEnergies)
			Scalar energy(int iSubEnergy, const Eigen::Matrix<Scalar, -1, 1>& x) const
			{
				return x.transpose() * _A * x - 2 * x.dot(_b.col(iSubEnergy)) + _c.coeff(iSubEnergy);
			}

			// Calculates the first subenergy for a given argument x.
			Scalar energy(const Eigen::Matrix<Scalar, -1, 1>& x) const
			{
				return energy(0, x);
			}

			// Calculates the change in energy at oldX when the specified variables are changed to a new value. The vector of
			// changed variables must be sorted by variable index.
			//   iSubEnergy: the index of the subenergy to calculate, must be in [0, SubEnergies)
			Scalar energyChange(int iSubEnergy, const Eigen::Matrix<Scalar, -1, 1>& oldX, const std::vector<std::pair<int, Scalar>>& changedX) const
			{
				Scalar change = 0;				
				for (auto& entry : changedX)
				{
					//iterator to the next changed value starting from the beginning of the current row
					auto nextChangedIt = changedX.begin();
					
					//calculate the change in the quadratic term
					for (Eigen::SparseMatrix<float>::InnerIterator it(_A, entry.first); it; ++it)
					{
						float factor = 1;
						auto oldContribution = oldX(entry.first) * oldX(it.index());
						auto newSecondValue = oldX(it.index());
						if (nextChangedIt != changedX.end() && nextChangedIt->first == it.index())
						{
							newSecondValue = nextChangedIt->second;							
							++nextChangedIt;
						}
						else
							factor = 2;
						auto newContribution = entry.second * newSecondValue;
						float coefficient = _A.coeff(entry.first, it.index());
						change += factor * _A.coeff(entry.first, it.index()) * (newContribution - oldContribution);
					}

					//calculate the change in the linear term
					change += -2 * _b(entry.first, iSubEnergy) * (entry.second - oldX(entry.first));
				}
				return change;
			}

			// Calculates the change in energy at oldX when the specified variables are changed to a new value
			Scalar energyChange(const Eigen::Matrix<Scalar, -1, 1>& oldX, const std::vector<std::pair<int, Scalar>>& changedX) const
			{
				return energyChange(0, oldX, changedX);
			}

			// Fixes a given variable to a given value in the energy. 
			void fixVariable(int idx, const Eigen::Matrix<Scalar, 1, SubEnergies>& value)
			{
				_A.uncompress();
				//Extract the variable from the quadratic part
				for (MatrixType::InnerIterator it(_A, idx); it; ++it)
				{
					if (it.index() == idx)
					{
						_c += value.cwiseProduct(value) * it.value();
					}
					else
					{
						_b.row(it.index()) -= it.value() * value;

						//remove the entry from the row
						int startId = _A.outerIndexPtr()[it.index()];
						int p = startId + _A.innerNonZeroPtr()[it.index()] - 1;
						auto removedValue = _A.valuePtr()[p];
						auto removedIndex = _A.innerIndexPtr()[p];
						while ((p > startId) && (removedIndex > idx))
						{
							--p;
							std::swap(removedValue, _A.valuePtr()[p]);
							std::swap(removedIndex, _A.innerIndexPtr()[p]);							
						}
						--_A.innerNonZeroPtr()[it.index()];
					}
				}
				//empty the row
				_A.innerNonZeroPtr()[idx] = 0;

				//Extract the variable from the linear part
				_c -= 2 * _b.row(idx).cwiseProduct(value);
				_b.row(idx).setZero();

				if (variableEliminated.size() == 0)
					variableEliminated.resize(_A.cols());
				variableEliminated[idx] = true;
			}

			// Fixes a given variable to a given value in the energy. 
			template <int Cols = SubEnergies>
			void fixVariable(int idx, typename std::enable_if<Cols == 1, Scalar>::type value)
			{
				Eigen::Matrix <Scalar, 1, 1> v;
				v(0) = value;
				fixVariable(idx, v);
			}

			// Adds a new quadratic energy term that is derived from a linear constraint to this energy.
			// The added energy is
			//   weight * [(Σ c_i * x_i) - b]^2
			// The first SkipColumns columns of the term's right-hand side are ignored.
			template <int SkipColumns, int Columns>
			void addTerm(const LinearConstraint<Columns, Scalar>& term, Scalar weight = 1)
			{
				for (auto& entry1 : term.lhsCoefficients)
				{
					for (auto& entry2 : term.lhsCoefficients)
					{
						_A.coeffRef(entry1.first, entry2.first) += weight * entry1.second * entry2.second;
					}
					_b.row(entry1.first) += weight * entry1.second * term.rhs.template block<SubEnergies, 1>(SkipColumns, 0).transpose();
				}
				_c += weight * term.rhs.template block<SubEnergies, 1>(SkipColumns, 0).transpose().cwiseProduct(term.rhs.template block<SubEnergies, 1>(SkipColumns, 0).transpose());
			}

			// Adds a new quadratic energy term that is derived from a linear constraint to this energy.
			// The added energy is
			//   weight * [(Σ c_i * x_i) - b]^2
			void addTerm(const LinearConstraint<SubEnergies, Scalar>& term, Scalar weight = 1)
			{
				addTerm<0>(term, weight);
			}

			// Has the same semantics as addTerm but performs all operations atomically. This requires
			// that all entries in the matrix are already present.
			template <int SkipColumns, int Columns>
			void addTermAtomic(const LinearConstraint<Columns, Scalar>& term, Scalar weight = 1)
			{
				Scalar* coeff = nullptr;
				for (auto& entry1 : term.lhsCoefficients)
				{
					if (entry1.second == 0)
						continue;
					for (auto& entry2 : term.lhsCoefficients)
					{
						if (entry2.second == 0)
							continue;
						coeff = &_A.coeffRef(entry1.first, entry2.first);
						nse::data::atomicAdd(coeff, weight * entry1.second * entry2.second);
					}
					for (int i = 0; i < SubEnergies; ++i)
					{
						coeff = &_b.coeffRef(entry1.first, i);
						nse::data::atomicAdd(coeff, weight * entry1.second * term.rhs.coeff(SkipColumns + i, 0));
					}
				}
				for (int i = 0; i < SubEnergies; ++i)
				{
					coeff = &_c.coeffRef(i);
					Scalar entry = term.rhs.coeff(SkipColumns + i, 0);
					nse::data::atomicAdd(coeff, weight * entry * entry);
				}
			}

			// Has the same semantics as addTerm but performs all operations atomically. This requires
			// that all entries in the matrix are already present.
			void addTermAtomic(const LinearConstraint<SubEnergies, Scalar>& term, Scalar weight = 1)
			{
				addTermAtomic<0>(term, weight);
			}	

			// Adds a Lagrange multiplier for a linear constraint. This will make the energy non-positive definite.
			void addConstraint(const LinearConstraint<SubEnergies, Scalar>& constraint)
			{
				auto cIdx = nextConstraint++;
				if (cIdx >= _A.rows())
				{
					_A.conservativeResize(_A.rows() + 1, _A.cols() + 1);
					_b.conservativeResize(_A.rows(), _b.cols());
				}

				//the variable in the energy is actually half the Lagrange multiplier
				for (auto& entry : constraint.lhsCoefficients)
				{
					_A.coeffRef(cIdx, entry.first) = entry.second;
					_A.coeffRef(entry.first, cIdx) = entry.second;
				}
				_b.row(cIdx) = constraint.rhs.transpose();
			}

			// Returns the number of variables (excluding Lagrange multipliers) of this energy
			size_t numberOfUnknowns() const { return unknowns; }

			// Returns the number of linear constraints that have been added to this energy.
			size_t numberOfConstraints() const { return nextConstraint - unknowns; }

			// Returns the quadratic part of the energy
			const MatrixType& A() const { return _A; }

			// Returns the linear part of the energy
			const Eigen::Matrix<Scalar, -1, SubEnergies>& b() const { return _b; }

			// Returns the constant part of this energy
			const Eigen::Matrix<Scalar, 1, SubEnergies>& c() const { return _c; }

			// Returns if a variable has been fixed by fixVariable()
			bool isVariableFixed(int idx) const { return !variableEliminated.empty() && variableEliminated[idx]; }

		private:
			// components of this energy
			MatrixType _A;
			Eigen::Matrix<Scalar, -1, SubEnergies> _b;
			Eigen::Matrix<Scalar, 1, SubEnergies> _c;

			// determines for each variable if it has been fixed
			std::vector<bool> variableEliminated;

			int unknowns = 0;
			int nextConstraint = 0;
		};
	}
}
#endif