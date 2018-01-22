/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author James Kanze
	@author Nico Schertler
*/

#include "nsessentials/util/IndentationLog.h"

using namespace nse::util;

IndentationLog nse::util::ilog(std::cout);

IndentationLog::IndentationLog(std::ostream& dest)
	: dest(dest.rdbuf())
	, isAtStartOfLine(true)
	, indent("  ")
	, owner(dest)
	, internalOutput(false)
{
	owner.rdbuf(this);	
}

void IndentationLog::startBlock()
{	
	blocks.emplace_back();
}

void IndentationLog::endBlock()
{	
	if (!blocks.back().hasContent)
	{
		internalOutput = true;
		owner << " ";
		internalOutput = false;
	}
	blocks.resize(blocks.size() - 1);	
}

int IndentationLog::overflow(int ch)
{
	if (blocks.size() > 0 && !internalOutput)
	{
		if (!blocks.back().hasContent)
		{
			dest->sputc('\n');
			isAtStartOfLine = true;
			blocks.back().hasContent = true;
		}
	}
	if (isAtStartOfLine && ch != '\n')
	{
		for (int i = 0; i < blocks.size(); ++i)
			dest->sputn(indent.data(), indent.size());
	}
	isAtStartOfLine = ch == '\n';
	return dest->sputc(ch);
}

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
