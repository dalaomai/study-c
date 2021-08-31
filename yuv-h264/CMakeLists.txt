cmake_minimum_required(VERSION 3.0.0)
project(yuv-h264)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(yuv-h264 main.cpp)
target_link_libraries(yuv-h264 avformat avutil avcodec)