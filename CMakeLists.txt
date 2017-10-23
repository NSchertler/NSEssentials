cmake_minimum_required (VERSION 2.8)

project("NSEssentials")

include_directories(include)

add_library(nsessentials STATIC			
			src/data/Parallelization.cpp
			
			src/gui/AbstractViewer.cpp			
			src/gui/Camera.cpp
			src/gui/GLBuffer.cpp
			src/gui/GLShader.cpp
			src/gui/GLVertexArray.cpp

			src/math/BoundingBox.cpp
			src/math/Morton.cpp

			src/util/IndentationLog.cpp
			src/util/Timer.cpp)