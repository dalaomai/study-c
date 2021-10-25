cmake_minimum_required(VERSION 3.0.0)
project(test)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(test main.cpp b.h b.cpp a.h)