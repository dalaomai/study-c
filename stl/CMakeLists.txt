cmake_minimum_required(VERSION 3.0.0)
project(stl)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(stl main.cpp autoptr.hpp)
