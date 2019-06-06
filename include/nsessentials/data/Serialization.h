/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <stdio.h>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <array>
#include <stdexcept>

#ifdef HAVE_EIGEN
#include <Eigen/Core>
#endif

namespace nse {
	namespace data
	{
		//Generic implementation
		template <typename T>
		void saveToFile(const T& object, FILE* f)
		{
			fwrite(&object, sizeof(T), 1, f);
		}

		template <typename T>
		void loadFromFile(T& object, FILE* f)
		{
			if(fread(&object, sizeof(T), 1, f) != 1)
				throw std::runtime_error("Cannot read enough data from file");
		}


		//std::vector
		template <typename T, typename Allocator>
		void saveToFile(const std::vector<T, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for (size_t i = 0; i < n; ++i)
				saveToFile(object[i], f);
		}

		template <typename T, typename Allocator>
		void loadFromFile(std::vector<T, Allocator>& object, FILE* f)
		{
			size_t n;
			loadFromFile(n, f);
			object.resize(n);
			for (size_t i = 0; i < n; ++i)
				loadFromFile(object[i], f);
		}


		//std::deque
		template <typename T, typename Allocator>
		void saveToFile(const std::deque<T, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for (size_t i = 0; i < n; ++i)
				saveToFile(object[i], f);
		}

		template <typename T, typename Allocator>
		void loadFromFile(std::deque<T, Allocator>& object, FILE* f)
		{
			size_t n;
			loadFromFile(n, f);
			object.resize(n);
			for (size_t i = 0; i < n; ++i)
				loadFromFile(object[i], f);
		}

		//std::array
		template <typename T, size_t Size>
		void saveToFile(const std::array<T, Size>& object, FILE* f)
		{
			for (size_t i = 0; i < Size; ++i)
				saveToFile(object[i], f);
		}

		template <typename T, size_t Size>
		void loadFromFile(std::array<T, Size>& object, FILE* f)
		{
			for (size_t i = 0; i < Size; ++i)
				loadFromFile(object[i], f);
		}

		//std::list
		template <typename T, typename Allocator>
		void saveToFile(const std::list<T, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for(auto& entry : object)
				saveToFile(entry, f);
		}

		template <typename T, typename Allocator>
		void loadFromFile(std::list<T, Allocator>& object, FILE* f)
		{
			size_t n;
			loadFromFile(n, f);
			object.clear();
			for (size_t i = 0; i < n; ++i)
			{
				object.emplace_back();
				nse::data::loadFromFile(object.back(), f);
			}
		}

		//std::map
		template <typename K, typename T, typename Pr, typename Allocator>
		void saveToFile(const std::map<K, T, Pr, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for (auto& p : object)
			{
				nse::data::saveToFile(p.first, f);
				nse::data::saveToFile(p.second, f);
			}
		}

		template <typename K, typename T, typename Pr, typename Allocator>
		void loadFromFile(std::map<K, T, Pr, Allocator>& object, FILE* f)
		{
			size_t n;
			nse::data::loadFromFile(n, f);
			object.clear();
			for(size_t i = 0; i < n; ++i)
			{
				K key;
				nse::data::loadFromFile(key, f);
				auto& entry = object[key];
				nse::data::loadFromFile(entry, f);
			}
		}

#ifdef HAVE_EIGEN
		template <typename T, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
		void saveToFile(const Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>& object, FILE* f)
		{
			if(Rows == Eigen::Dynamic)
				saveToFile(object.rows(), f);
			if(Cols == Eigen::Dynamic)
				saveToFile(object.cols(), f);
			for (int c = 0; c < object.cols(); ++c)
				for (int r = 0; r < object.rows(); ++r)
					saveToFile(object.coeff(r, c), f);
		}

		template <typename T, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
		void loadFromFile(Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>& object, FILE* f)
		{
			decltype(object.rows()) rows, cols;
			if (Rows == Eigen::Dynamic)
				loadFromFile(rows, f);
			else
				rows = Rows;

			if (Cols == Eigen::Dynamic)
				loadFromFile(cols, f);
			else
				cols = Cols;

			object.resize(rows, cols);
			for (int c = 0; c < object.cols(); ++c)
				for (int r = 0; r < object.rows(); ++r)
					loadFromFile(object.coeffRef(r, c), f);
		}
#endif
	}
}