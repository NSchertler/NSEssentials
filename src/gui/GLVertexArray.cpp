/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#ifdef HAVE_NANOGUI

#include "nsessentials/gui/GLVertexArray.h"

using namespace nse::gui;

GLVertexArray::GLVertexArray()
	: vaoId(0)
{
}

GLVertexArray::~GLVertexArray()
{
	if (vaoId > 0)
		glDeleteVertexArrays(1, &vaoId);
}

void GLVertexArray::generate()
{
	if (vaoId == 0)
		glGenVertexArrays(1, &vaoId);
}

void GLVertexArray::bind() const
{
	glBindVertexArray(vaoId);
}

void GLVertexArray::unbind() const
{
	glBindVertexArray(0);
}

bool GLVertexArray::valid() const
{
	return vaoId != 0;
}

#endif