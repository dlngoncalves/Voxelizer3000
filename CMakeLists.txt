cmake_minimum_required (VERSION 3.12)
project (Voxelizer3000)

if(APPLE)
	find_package(OpenGL REQUIRED)
	find_package(GLUT REQUIRED)
	
	if(OPENGL_FOUND)
	    link_directories(${OpenGL_LIBRARY_DIRS})
	    include_directories(${OpenGL_INCLUDE_DIR})
	endif(OPENGL_FOUND)
	
	if(GLUT_FOUND)
    	    link_directories(${GLUT_LIBRARY_DIR})
    	    include_directories(${GLUT_INCLUDE_DIR})
	endif(GLUT_FOUND)
endif(APPLE)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(Voxelizer3000)

