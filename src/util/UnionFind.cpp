#include "nsessentials/util/UnionFind.h"

#include <stdio.h>

using namespace nse::util;

void UnionFind::SaveToFile(const char* filename) const
{
	FILE* file = fopen(filename, "wb");
	uint64_t entries = size();
	fwrite(&entries, sizeof(uint64_t), 1, file);
	fwrite(&parentIndices[0], sizeof(index_t), parentIndices.size(), file);
	fwrite(&ranks[0], sizeof(unsigned int), ranks.size(), file);
	fclose(file);
}

size_t UnionFind::size() const { return parentIndices.size(); }

// Loads the entire structure from a file. Existing data in the structure is overridden.
void UnionFind::LoadFromFile(const char* filename)
{
	uint64_t entries;

	FILE* file = fopen(filename, "rb");
	fread(&entries, sizeof(uint64_t), 1, file);
	parentIndices.resize(entries);
	ranks.resize(entries);
	fread(&parentIndices[0], sizeof(index_t), entries, file);
	fread(&ranks[0], sizeof(unsigned int), entries, file);
	fclose(file);
}

// Adds an item to the structure
void UnionFind::AddItem()
{
	parentIndices.push_back((index_t)parentIndices.size());
	ranks.push_back(0);
}

void UnionFind::AddItems(size_t count)
{
	auto oldCount = parentIndices.size();
	parentIndices.resize(parentIndices.size() + count);
	for (int i = oldCount; i < parentIndices.size(); ++i)
		parentIndices[i] = i;
	ranks.resize(ranks.size() + count);
}

void UnionFind::Clear()
{
	parentIndices.clear();
	ranks.clear();
}

// Finds the set representative for a given entry. Two entries are in the same set
// iff they have the same set representative.
UnionFind::index_t UnionFind::GetRepresentative(index_t index)
{
	//Find the root
	index_t current = index;
	while (parentIndices[current] != current)
		current = parentIndices[current];

	//Path compression
	index_t root = current;
	current = index;
	while (parentIndices[current] != current)
	{
		index_t i = current;
		current = parentIndices[current];
		parentIndices[i] = root;
	}

	return root;
}

// Merges the sets of the two specified entries.
UnionFind::index_t UnionFind::Merge(index_t i1, index_t i2)
{
	index_t rep1 = GetRepresentative(i1);
	index_t rep2 = GetRepresentative(i2);
	if (rep1 == rep2)
		return rep1;

	//Union by rank
	unsigned int rank1 = ranks[rep1];
	unsigned int rank2 = ranks[rep2];

	if (rank1 < rank2)
	{
		ConcreteMerge(rep2, rep1);
		return rep2;
	}
	else if (rank2 < rank1)
	{
		ConcreteMerge(rep1, rep2);
		return rep1;
	}
	else
	{
		ConcreteMerge(rep1, rep2);
		++ranks[rep1];
		return rep1;
	}
}

void UnionFind::ConcreteMerge(index_t newRoot, index_t child)
{
	parentIndices[child] = newRoot;
}