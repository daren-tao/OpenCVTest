cmake_minimum_required (VERSION 2.6)

project(opencv_tutorials)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS -std=c++11)

set(exe_name imageproc_module)

find_package(OpenCV REQUIRED)

include_directories(
	${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

link_directories(
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin/)
add_executable(${exe_name} ${PROJECT_SOURCE_DIR}/demo/videoCapture.cpp)

target_link_libraries(${exe_name} 
	${OpenCV_LIBS}
)
