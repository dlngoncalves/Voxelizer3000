#cmake_minimum_required (VERSION 3.12)
#project (Voxelizer3000)

set(CMAKE_CXX_STANDARD 14)
add_executable (Voxelizer3000 demo.cpp objLoader.cpp vertex.h)

target_link_libraries(Voxelizer3000 ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
