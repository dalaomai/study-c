cmake_minimum_required(VERSION 3.12)
project(studyGrpc LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(gRPC_DIR "/root/study_c/grpc/installed/lib/cmake/grpc")
set(Protobuf_DIR "/root/study_c/grpc/installed/lib64/cmake/protobuf")

find_package(Threads REQUIRED)

find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

include_directories(${GRPC_INCLUDE_DIR})

add_library(testProto OBJECT proto/test.grpc.pb.cc proto/test.grpc.pb.h proto/test.pb.h proto/test.pb.cc)

add_executable(studyGrpcServer server.cpp)
add_executable(studyGrpc main.cpp)
add_executable(studyGrpcAsyncServer asyncServer.cpp)

foreach(_target "studyGrpcServer" "studyGrpc" "studyGrpcAsyncServer")
target_link_libraries(${_target} gRPC::grpc++ gRPC::grpc++_reflection $<TARGET_OBJECTS:testProto>)
endforeach()

