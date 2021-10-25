cmake_minimum_required(VERSION 3.12)

project(socket LANGUAGES CXX)

add_executable(server server.cpp)
add_executable(client client.cpp)