#include <string>
#include <grpc++/grpc++.h>
#include "proto/test.grpc.pb.h"
#include <time.h>

class TestClient
{
public:
    TestClient(std::shared_ptr<grpc::Channel> channel) : stub(TestRoute::NewStub(channel)){};

    int GetTest(int a)
    {

        grpc::ClientContext context;
        Input input;
        OutPut output;

        input.set_a(a);

        grpc::Status status = stub->GetTest(&context, input, &output);

        if (status.ok())
        {
            return output.b();
        }
        else
        {
            return -1;
        }
    }

    void GetListTest()
    {
        grpc::ClientContext context;
        std::unique_ptr<grpc::ClientReaderWriter<Input, OutPut>> rw = stub->GetStreamTest(&context);

        Input tmpInput;
        OutPut tmpOutput;
        for (int i = 0; i < 10; i++)
        {
            tmpInput.set_a(i);
            rw->Write(tmpInput);
        }
        rw->WritesDone();

        while (rw->Read(&tmpOutput))
        {
            std::cout << tmpOutput.b() << std::endl;
        }
        rw->Finish();
    }

    int AsyncGetTest(int a)
    {
        grpc::ClientContext context;
        grpc::CompletionQueue cq;

        Input input;
        input.set_a(a);

        std::unique_ptr<grpc::ClientAsyncResponseReader<OutPut>> asyncRsp = stub->PrepareAsyncGetTest(&context, input, &cq);

        asyncRsp->StartCall();

        OutPut output;
        grpc::Status status;
        asyncRsp->Finish(&output, &status, &output);

        void *got_tag;
        bool ok = false;
        GPR_ASSERT(cq.Next(&got_tag, &ok));
        GPR_ASSERT(ok);
        GPR_ASSERT(got_tag == &output);

        if (!status.ok())
        {
            return -1;
        }
        return static_cast<OutPut *>(got_tag)->b();
    }

private:
    std::shared_ptr<TestRoute::Stub> stub;
};

int main()
{
    std::string server_host = "0.0.0.0:5001";
    TestClient client(grpc::CreateChannel(server_host, grpc::InsecureChannelCredentials()));

    // std::cout << client.GetTest(2) << std::endl;
    std::cout << client.AsyncGetTest(2) << std::endl;
    std::cout << "Use time: " << clock() << std::endl;
    // client.GetListTest();
}