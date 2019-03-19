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
		//Represents a pseudo-iterator that can be used to infinitely cycle over
		//an underlying sequential iterator range. When the circulator hits the
		//end of the sequence, it restarts at the beginning.
		template <typename Iterator>
		class Circulator
		{
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef typename std::iterator_traits<Iterator>::value_type value_type;
			typedef size_t difference_type;
			typedef typename std::iterator_traits<Iterator>::pointer pointer;
			typedef typename std::iterator_traits<Iterator>::reference reference;			

			Circulator(Iterator current, Iterator sequenceBegin, Iterator sequenceEnd)
				: current(current), begin(sequenceBegin), end(sequenceEnd)
			{
				if (this->current == end)
					this->current = begin;
			}

			Circulator& operator++() { ++current; if (current == end) current = begin; return *this; }
			bool operator==(const Circulator& other) const { return other.current == current; }
			bool operator!=(const Circulator& other) const { return !(*this == other); }
			value_type operator*() const { return *current; }

		protected:
			Iterator current, begin, end;
		};		

		template <typename Iterator>
		Circulator<Iterator> MakeCirculator(Iterator current, Iterator sequenceBegin, Iterator sequenceEnd)
		{
			return Circulator<Iterator>(current, sequenceBegin, sequenceEnd);
		}
	}
}