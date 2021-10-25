protoc --grpc_out=./proto --plugin=protoc-gen-grpc=/root/study_c/grpc/installed/bin/grpc_cpp_plugin ./test.proto

protoc --cpp_out=./proto ./test.proto