#include <string>
#include <grpc++/grpc++.h>
#include "proto/test.grpc.pb.h"

class ServerImpl final
{
public:
    ServerImpl(std::string server_host) : server_host(server_host){};

    ~ServerImpl()
    {
        server->Shutdown();
        cq->Shutdown();
    }

    void Run()
    {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_host, grpc::InsecureServerCredentials());
        std::cout << "Async Server Listering on" << server_host << std::endl;
        builder.RegisterService(&service);
        cq = builder.AddCompletionQueue();
        server = builder.BuildAndStart();

        HandleRpcs();
    }

private:
    std::string server_host;
    std::unique_ptr<grpc::ServerCompletionQueue> cq;
    TestRoute::AsyncService service;
    std::unique_ptr<grpc::Server> server;

    class CallGetTest
    {
    public:
        CallGetTest(TestRoute::AsyncService *service, grpc::ServerCompletionQueue *cq, int index) : service(service), cq(cq), status(CREATE), responder(&ctx), index(index)
        {
            Proceed();
        };

        void Proceed()
        {
            if (status == CREATE)
            {
                status = PROCESS;
                service->RequestGetTest(&ctx, &input, &responder, cq, cq, this);
            }
            else if (status == PROCESS)
            {
                new CallGetTest(service, cq, index);

                output.set_b(input.a() + index);
                responder.Finish(output, grpc::Status::OK, this);

                status = FINISH;
            }
            else
            {
                delete this;
            }
        };

    private:
        int index;
        TestRoute::AsyncService *service;
        grpc::ServerCompletionQueue *cq;
        grpc::ServerContext ctx;
        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status;
        Input input;
        OutPut output;
        grpc::ServerAsyncResponseWriter<OutPut> responder;
    };

    void HandleRpcs()
    {
        new CallGetTest(&service, cq.get(), 1);
        new CallGetTest(&service, cq.get(), 2);

        void *tag;
        bool ok;
        while (true)
        {
            cq->Next(&tag, &ok);
            static_cast<CallGetTest *>(tag)->Proceed();
        }
    }
};

int main()
{
    std::string server_host = "0.0.0.0:5001";
    ServerImpl server(server_host);
    server.Run();
}