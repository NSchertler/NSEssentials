/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <iterator>

#include <nsessentials/util/IteratorRange.h>

namespace nse
{
	namespace util
	{
		// Iterator that references a position within a container hierarchy. The iterator can be used to
		// sequentially iterate elements on a given level of the hierarchy.
		// E.g., the integer values of a std::vector<std::vector<int>> can be iterated using 
		//   Iterator = std::vector<std::vector<int>>::iterator  (the top-level iterator type)
		//   FlatteningLevels = 1   (how many levels to go down the hierarchy)
		template <typename Iterator, int FlatteningLevels = 1>
		class FlatteningIterator
		{
			typedef typename FlatteningIterator<Iterator, FlatteningLevels - 1> ParentFlattening;
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef typename std::iterator_traits<typename ParentFlattening::value_type::iterator>::value_type value_type;
			typedef size_t difference_type;
			typedef typename std::iterator_traits<typename ParentFlattening::value_type::iterator>::pointer pointer;
			typedef typename std::iterator_traits<typename ParentFlattening::value_type::iterator>::reference reference;

			FlatteningIterator() { }
			FlatteningIterator(Iterator topLevelCurrent, Iterator topLevelEnd)
				: parentIterator(topLevelCurrent, topLevelEnd)
			{
				if (!parentIterator.isAtEnd())
					leafIterator = parentIterator->begin();
			}

			FlatteningIterator& operator++()
			{
				++leafIterator;
				while (leafIterator == parentIterator->end())
				{
					++parentIterator;
					if (!parentIterator.isAtEnd())
						leafIterator = parentIterator->begin();
					else
						break;
				}
				return *this;
			}

			bool operator==(const FlatteningIterator<Iterator, FlatteningLevels>& other) const
			{
				if (isAtEnd())
					return other.isAtEnd();
				return !other.isAtEnd() 
					&& parentIterator == other.parentIterator 
					&& leafIterator == other.leafIterator;
			}
			bool operator!=(const FlatteningIterator& other) const { return !(*this == other); }
			reference operator*() const { return *leafIterator; }
			pointer operator->() const { return &(*leafIterator); }

			bool isAtEnd() const { return parentIterator.isAtEnd(); }

		private:

			typename ParentFlattening::value_type::iterator leafIterator;
			ParentFlattening parentIterator;
		};

		// Base iterator for the top level of a hierarchy flattening
		template <typename Iterator>
		class FlatteningIterator<Iterator, 0>
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef typename std::iterator_traits<Iterator>::value_type value_type;
			typedef size_t difference_type;
			typedef typename std::iterator_traits<Iterator>::pointer pointer;
			typedef typename std::iterator_traits<Iterator>::reference reference;

			FlatteningIterator() { }
			FlatteningIterator(Iterator topLevelCurrent, Iterator topLevelEnd)
				: current(topLevelCurrent), end(topLevelEnd)
			{ }

			FlatteningIterator& operator++()
			{
				++current;
				return *this;
			}

			bool operator==(const FlatteningIterator& other) const { return current == other.current; }
			bool operator!=(const FlatteningIterator& other) const { return !(*this == other); }
			reference operator*() const { return *current; }
			pointer operator->() const { return &(*current); }

			bool isAtEnd() const { return current == end; }

		private:
			Iterator current, end;
		};

		// Returns a range object that sequentially iterate elements on a given level of a
		// container hierarchy, e.g. a std::vector<std::vector<int>>.
		// begin: the begin iterator of the top-level container
		// end: the end iterator of the top-level container
		template <int FlatteningLevels = 1, typename Iterator>
		IteratorRange<FlatteningIterator<Iterator, FlatteningLevels>> FlattenHierarchy(Iterator begin, Iterator end)
		{
			FlatteningIterator<Iterator, FlatteningLevels> flattenedBegin(begin, end);
			FlatteningIterator<Iterator, FlatteningLevels> flattenedEnd(end, end);

			return MakeIteratorRange(flattenedBegin, flattenedEnd);
		}
	}
}