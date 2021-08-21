cmake_minimum_required(VERSION 3.0.0)

project(ffmpeg-version)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILES main.cpp)

add_executable(ffmpeg-version ${SOURCE_FILES})
target_link_libraries(ffmpeg-version avcodec)