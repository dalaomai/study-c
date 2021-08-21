cmake_minimum_required(VERSION 3.0.0)
project(video-info)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_executable(video-info main.cpp)
target_link_libraries(video-info avformat)