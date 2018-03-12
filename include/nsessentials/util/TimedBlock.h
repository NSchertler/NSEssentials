/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <nsessentials/util/Timer.h>

namespace nse {
	namespace util
	{		
		//RAII style object that indents all output during its lifetime. Furthermore,
		//measures the time until destruction and outputs it.
		struct TimedBlock
		{
			//s - Initial output of the block (usually used to name the current operation).
			//highPriority - if REDUCED_TIMINGS is defined, omits all blocks with highPriority=false
			TimedBlock(const std::string& s, bool highPriority = false);
			~TimedBlock();

			void closeBlock();
			size_t time() const;
		private:

			bool exited;
			nse::util::Timer<> t;
		};
	}
}