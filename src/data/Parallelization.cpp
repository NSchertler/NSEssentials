/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Wenzel Jakob
*/

#include "nsessentials/data/Parallelization.h"

using namespace nse::data;

ordered_lock::ordered_lock() : next_ticket(0), counter(0) {}
void ordered_lock::lock()
{
	std::unique_lock<std::mutex> acquire(cvar_lock);
	unsigned int ticket = next_ticket++;
	while (ticket != counter)
		cvar.wait(acquire);
}
void ordered_lock::unlock()
{
	std::unique_lock<std::mutex> acquire(cvar_lock);
	counter++;
	cvar.notify_all();
}