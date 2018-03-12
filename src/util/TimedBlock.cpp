/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#include "nsessentials/util/TimedBlock.h"
#include <iostream>
#include "nsessentials/util/IndentationLog.h"

using namespace nse::util;

TimedBlock::TimedBlock(const std::string & s, bool highPriority)
	: exited(false)
{
#ifdef REDUCED_TIMINGS
	if (!highPriority)
	{
		exited = true;
		return;
	}
#endif
	std::cout << s;
	ilog.startBlock();
}

TimedBlock::~TimedBlock()
{
	closeBlock();
}

size_t TimedBlock::time() const
{
	return t.value();
}

void TimedBlock::closeBlock()
{
	if (exited)
		return;
	auto time = t.value();
	ilog.endBlock();
	exited = true;
	std::cout << "done (took " << timeString((double)time) << ")." << std::endl;
}
