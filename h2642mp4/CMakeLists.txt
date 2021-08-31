cmake_minimum_required(VERSION 3.0.0)
project(h2642mp4)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(h2642mp4 main.cpp)
target_link_libraries(h2642mp4 avformat avutil avcodec)