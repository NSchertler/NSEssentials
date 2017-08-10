cmake_minimum_required (VERSION 2.8)

project("NSEssentials")

include_directories(include)

SET(NSE_EXTRA_DEFS)

option(NSE_WITH_TBB "Specify to compile with TBB. The include directory should be added by the parent project.")
if(NSE_WITH_TBB)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_TBB)
endif()

option(NSE_WITH_EIGEN "Specify to compile with Eigen. The include directory should be added by the parent project.")
if(NSE_WITH_EIGEN)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_EIGEN)
endif()

option(NSE_WITH_NANOGUI "Specify to compile with NanoGUI. The include directory should be added by the parent project.")
if(NSE_WITH_NANOGUI)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_NANOGUI)
endif()

add_definitions(${NSE_EXTRA_DEFS})
SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} PARENT_SCOPE)

add_library(nsessentials STATIC			
			src/data/Parallelization.cpp
			
			src/gui/AbstractViewer.cpp			
			src/gui/Camera.cpp
			src/gui/GLBuffer.cpp
			src/gui/GLShader.cpp
			src/gui/GLVertexArray.cpp

			src/math/BoundingBox.cpp
			src/math/LeastSquaresSystem.cpp
			src/math/Morton.cpp

			src/util/IndentationLog.cpp
			src/util/Timer.cpp)