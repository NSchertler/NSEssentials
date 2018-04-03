/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Wenzel Jakob
	@author Nico Schertler
*/

#pragma once

#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

#include "nsessentials/NSELibrary.h"

#if !(defined(_MSC_VER) && _MSC_VER < 1910)
//This explicit template instantation and export does not work under Visual Studio <= 2015
template class NSE_EXPORT std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>;
template class NSE_EXPORT std::chrono::time_point<std::chrono::system_clock>;
#endif

namespace nse {
	namespace util
	{
		template <typename TimeT = std::chrono::milliseconds>
		class NSE_EXPORT Timer
		{
		public:
			Timer()
			{
				start = std::chrono::system_clock::now();
			}

			size_t value() const
			{
				auto now = std::chrono::system_clock::now();
				auto duration = std::chrono::duration_cast<TimeT>(now - start);
				return (size_t)duration.count();
			}

			size_t reset()
			{
				auto now = std::chrono::system_clock::now();
				auto duration = std::chrono::duration_cast<TimeT>(now - start);
				start = now;
				return (size_t)duration.count();
			}
		private:
			std::chrono::system_clock::time_point start;
		};

		template class NSE_EXPORT Timer<>;

		//Converts the provided number of milliseconds to a readable string.
		//If precise=true, uses milliseconds. If precise=false, converts to 
		//an appropriate unit.
		extern NSE_EXPORT ::std::string timeString(double time, bool precise = false);
	}
}