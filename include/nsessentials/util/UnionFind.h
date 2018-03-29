#pragma once

#include <vector>
#include <cstddef>

#include "nsessentials/NSELibrary.h"

namespace nse
{
	namespace util
	{
		// Represents a union-find data structure (disjoint sets).
		class UnionFind
		{
			// Node is root iff parent index == index

		public:
			typedef unsigned int index_t;

			// Saves the entire structure to a file for later usage
			NSE_EXPORT void SaveToFile(const char* filename) const;

			// Returns the number of entries
			NSE_EXPORT std::size_t size() const;

			// Loads the entire structure from a file. Existing data in the structure is overridden.
			NSE_EXPORT void LoadFromFile(const char* filename);

			// Adds an item to the structure
			NSE_EXPORT void AddItem();

			NSE_EXPORT void AddItems(std::size_t count);

			NSE_EXPORT void Clear();

			// Finds the set representative for a given entry. Two entries are in the same set
			// iff they have the same set representative.
			NSE_EXPORT index_t GetRepresentative(index_t index);

			// Merges the sets of the two specified entries. Returns the new root.
			NSE_EXPORT index_t Merge(index_t i1, index_t i2);

			// Merges the sets of the two specified entries, such that the specified entry will be the new root of the subtree.
			//newRoot must already be the representative of itself.
			NSE_EXPORT void MergeWithPredefinedRoot(index_t newRoot, index_t i);

		private:
			void ConcreteMerge(index_t newRoot, index_t child);

			std::vector<index_t> parentIndices;
			std::vector<unsigned int> ranks;
		};
	}
}