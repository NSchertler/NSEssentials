#pragma once

#ifdef HAVE_NANOGUI

#include <nanogui/opengl.h>
#include <unordered_set>

#include "nsessentials/NSELibrary.h"

namespace nse
{
	namespace util
	{
		class NSE_EXPORT GLDebug
		{
		public:
			static void IgnoreGLError(GLuint errorId);
			static void SetupDebugCallback();
			
		private:
			static std::unordered_set<GLuint> ignoredIds;
		};		
	}
}

#endif