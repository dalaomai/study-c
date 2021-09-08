cmake_minimum_required(VERSION 3.0.0)
project(template-test)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(template-test main.cpp utils.h )
target_link_libraries(template-test pthread)