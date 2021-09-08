cmake_minimum_required(VERSION 3.0.0)
project(audio-resample)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(audio-resample main.cpp)
target_link_libraries(audio-resample avformat avutil avcodec swresample)