#include "proto/test.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <string>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <iostream>

class TestServiceImpl final : public TestRoute::Service
{
    grpc::Status GetTest(grpc::ServerContext *context, const Input *request, OutPut *response) override
    {
        int get = request->a();
        response->set_b(get + 1);
        return grpc::Status::OK;
    }

    ::grpc::Status GetStreamTest(::grpc::ServerContext *context, ::grpc::ServerReaderWriter<::OutPut, ::Input> *stream) override
    {

        Input tmpInput;
        OutPut tmpOutput;
        while (stream->Read(&tmpInput))
        {
            tmpOutput.set_b(tmpInput.a() + 1);
            stream->Write(tmpOutput);
        }
        return grpc::Status::OK;
    }
};

void RunServer()
{
    std::string server_address = "0.0.0.0:5001";
    TestServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listrening on " << server_address << std::endl;

    server->Wait();
}

int main()
{
    RunServer();
}