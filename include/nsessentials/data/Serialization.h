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

namespace nse {
	namespace data
	{

		template <typename T>
		void saveToFile(const T& object, FILE* f)
		{
			fwrite(&object, sizeof(T), 1, f);
		}

		template <typename T>
		void loadFromFile(T& object, FILE* f)
		{
			fread(&object, sizeof(T), 1, f);
		}

		template <typename T, typename Allocator>
		void saveToFile(const std::vector<T, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for (int i = 0; i < n; ++i)
				saveToFile(object[i], f);
		}

		template <typename T, typename Allocator>
		void loadFromFile(std::vector<T, Allocator>& object, FILE* f)
		{
			size_t n;
			loadFromFile(n, f);
			object.resize(n);
			for (int i = 0; i < n; ++i)
				loadFromFile(object[i], f);
		}


		template <typename T, typename Allocator>
		void saveToFile(const std::deque<T, Allocator>& object, FILE* f)
		{
			size_t n = object.size();
			saveToFile(n, f);
			for (int i = 0; i < n; ++i)
				saveToFile(object[i], f);
		}

		template <typename T, typename Allocator>
		void loadFromFile(std::deque<T, Allocator>& object, FILE* f)
		{
			size_t n;
			loadFromFile(n, f);
			object.resize(n);
			for (int i = 0; i < n; ++i)
				loadFromFile(object[i], f);

		}
	}
}