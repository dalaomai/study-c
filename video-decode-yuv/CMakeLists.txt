cmake_minimum_required(VERSION 3.0.0)
project(video-decode-yuv)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(video-decode main.cpp)
target_link_libraries(video-decode avformat avcodec avutil swscale)