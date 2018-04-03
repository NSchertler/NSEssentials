cmake_minimum_required (VERSION 2.8)

project("NSEssentials")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/")

option(NSE_BUILD_SHARED "Build NSEssentials as a shared library" OFF)

SET(NSE_EXTRA_DEFS)
SET(LIBS)

if(NSE_BUILD_SHARED)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DNSE_BUILD_SHARED)
endif()

include_directories(include)

option(NSE_WITH_TBB "Specify to compile with TBB. The include directory should be added by the parent project. The target tbb must exist.")
if(NSE_WITH_TBB)	
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_TBB)

	if(NSE_BUILD_SHARED)		
		set(LIBS ${LIBS} tbb)
	endif()
endif()

option(NSE_WITH_EIGEN "Specify to compile with Eigen. The include directory should be added by the parent project.")
if(NSE_WITH_EIGEN)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_EIGEN)	
endif()

option(NSE_WITH_NANOGUI "Specify to compile with NanoGUI. The include directory should be added by the parent project. The target nanogui ${NANOGUI_EXTRA_LIBS} must exist.")
if(NSE_WITH_NANOGUI)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_NANOGUI)
	if(NSE_BUILD_SHARED)		
		set(LIBS ${LIBS} nanogui ${NANOGUI_EXTRA_LIBS})
	endif()
endif()

option(NSE_SSBO_SUPPORT "Specify to compile with support for shader storage buffers.")
if(NSE_SSBO_SUPPORT)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_SSBO)
endif()

option(NSE_TESSELLATION_SUPPORT "Specify to compile with support for tessellation shaders.")
if(NSE_TESSELLATION_SUPPORT)
	SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} /DHAVE_TESSELLATION)
endif()

add_definitions(${NSE_EXTRA_DEFS})
SET(NSE_EXTRA_DEFS ${NSE_EXTRA_DEFS} PARENT_SCOPE)

if(NSE_BUILD_SHARED)
	set(NSE_BUILD_TYPE "SHARED")
else()
	set(NSE_BUILD_TYPE "STATIC")
endif()

add_library(nsessentials ${NSE_BUILD_TYPE}			
			src/data/FileHelper.cpp
			src/data/Parallelization.cpp
			
			src/gui/AbstractViewer.cpp			
			src/gui/Camera.cpp
			src/gui/GLBuffer.cpp
			src/gui/GLShader.cpp
			src/gui/GLVertexArray.cpp

			src/math/BoundingBox.cpp
			src/math/Morton.cpp

			src/util/IndentationLog.cpp
			src/util/TimedBlock.cpp
			src/util/Timer.cpp
			src/util/GLDebug.cpp
			src/util/UnionFind.cpp)

if(NSE_BUILD_SHARED)
	target_link_libraries(nsessentials ${LIBS})
endif()