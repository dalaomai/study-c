cmake_minimum_required(VERSION 3.0.0)
project(rtsp-local)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(rtsp-local main.cpp  utils.h rtsprecord.cpp rtsprecord.h)
target_link_libraries(rtsp-local avformat avutil avcodec swscale swresample pthread)