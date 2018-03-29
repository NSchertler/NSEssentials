/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author James Kanze
	@author Nico Schertler
*/

//Based on https://stackoverflow.com/a/9600752/1210053

#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "nsessentials/NSELibrary.h"

namespace nse {
	namespace util
	{
		//Alternative stream buffer that indents all output by a specified amount.
		class IndentationLog : public std::streambuf
		{
		public:

			NSE_EXPORT IndentationLog(std::ostream& dest);

			//Starts a block, such that all further output is indented.
			NSE_EXPORT void startBlock();

			//Ends an indentation block
			NSE_EXPORT void endBlock();

		protected:
			virtual int overflow(int ch);

		private:
			std::streambuf*     dest;
			bool                isAtStartOfLine;
			std::ostream&       owner;
			std::string indent;

			struct BlockInfo
			{
				BlockInfo()
					: hasContent(false)
				{ }

				bool hasContent;
			};

			bool internalOutput;
			std::vector<BlockInfo> blocks;
		};
		extern NSE_EXPORT IndentationLog ilog;
	}
}