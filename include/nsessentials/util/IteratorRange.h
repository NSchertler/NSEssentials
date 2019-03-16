/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <iterator>

namespace nse
{
	namespace util
	{
		template <typename Iterator>
		class IteratorRangeBase
		{
		public:

			typedef Iterator iterator;

			IteratorRangeBase(Iterator begin, Iterator end)
				: _begin(begin), _end(end)
			{ }

			Iterator begin() const { return _begin; }
			Iterator end() const { return _end; }

		protected:
			Iterator _begin, _end;
		};

		//Helper class to wrap a pair of iterators in a structure that can be iterated with
		//a range-based for loop.
		template <typename Iterator, typename IteratorCategory = typename std::iterator_traits<Iterator>::iterator_category>
		class IteratorRange
			: public IteratorRangeBase<Iterator>
		{			
		public:
			IteratorRange(Iterator begin, Iterator end)
				: IteratorRangeBase(begin, end)
			{ }
		};

		//Helper class to wrap a pair of iterators in a structure that can be iterated with
		//a range-based for loop. Additionally, helper methods for random access are available.
		template <typename Iterator>
		class IteratorRange<Iterator, std::random_access_iterator_tag>
			: public IteratorRangeBase<Iterator>
		{
		public:
			IteratorRange(Iterator begin, Iterator end)
				: IteratorRangeBase(begin, end)
			{ }

			typename std::iterator_traits<Iterator>::difference_type size() const { return _end - _begin; }
			typename std::iterator_traits<Iterator>::reference operator[](int i) const { return *(_begin + i); }
		};

		template <typename Iterator>
		IteratorRange<Iterator> MakeIteratorRange(Iterator begin, Iterator end)
		{
			return IteratorRange<Iterator>(begin, end);
		}
	}
}