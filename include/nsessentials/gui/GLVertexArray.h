/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#ifdef HAVE_NANOGUI
#include <nanogui/opengl.h>

#include "nsessentials/NSELibrary.h"

namespace nse {
	namespace gui
	{
		//Represents an OpenGL Vertex Array Object
		class NSE_EXPORT GLVertexArray
		{
		public:
			GLVertexArray();
			~GLVertexArray();

			//Generates the VAO if it has not been generated yet.
			void generate();

			//Binds the VAO.
			void bind() const;

			//Binds a zero VAO.
			void unbind() const;

			//Returns if this VAO has been generated.
			bool valid() const;

		private:
			GLuint vaoId;
		};
	}
}
#endif