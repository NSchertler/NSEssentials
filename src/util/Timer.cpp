/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Wenzel Jakob
	@author Nico Schertler
*/

#include "nsessentials/util/Timer.h"

#include <cmath>

using namespace nse::util;

namespace nse
{
	namespace util
	{
		template class Timer<>;
	}
}

std::string nse::util::timeString(double time, bool precise)
{
	if (std::isnan(time) || std::isinf(time))
		return "inf";

	std::string suffix = "ms";
#ifndef ACCURATE_TIMING
	if (time > 1000) {
		time /= 1000; suffix = "s";
		if (time > 60) {
			time /= 60; suffix = "m";
			if (time > 60) {
				time /= 60; suffix = "h";
				if (time > 24) {
					time /= 24; suffix = "d";
				}
			}
		}
	}
#endif

	std::ostringstream os;
	os << std::setprecision(precise ? 4 : 1)
		<< std::fixed << time << suffix;

	return os.str();
}
