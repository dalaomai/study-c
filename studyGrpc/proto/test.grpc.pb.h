// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: test.proto
#ifndef GRPC_test_2eproto__INCLUDED
#define GRPC_test_2eproto__INCLUDED

#include "test.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

class TestRoute final {
 public:
  static constexpr char const* service_full_name() {
    return "TestRoute";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status GetTest(::grpc::ClientContext* context, const ::Input& request, ::OutPut* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>> AsyncGetTest(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>>(AsyncGetTestRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>> PrepareAsyncGetTest(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>>(PrepareAsyncGetTestRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::Input, ::OutPut>> GetStreamTest(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::Input, ::OutPut>>(GetStreamTestRaw(context));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>> AsyncGetStreamTest(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>>(AsyncGetStreamTestRaw(context, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>> PrepareAsyncGetStreamTest(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>>(PrepareAsyncGetStreamTestRaw(context, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void GetTest(::grpc::ClientContext* context, const ::Input* request, ::OutPut* response, std::function<void(::grpc::Status)>) = 0;
      virtual void GetTest(::grpc::ClientContext* context, const ::Input* request, ::OutPut* response, ::grpc::ClientUnaryReactor* reactor) = 0;
      virtual void GetStreamTest(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::Input,::OutPut>* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>* AsyncGetTestRaw(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::OutPut>* PrepareAsyncGetTestRaw(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderWriterInterface< ::Input, ::OutPut>* GetStreamTestRaw(::grpc::ClientContext* context) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>* AsyncGetStreamTestRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::Input, ::OutPut>* PrepareAsyncGetStreamTestRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status GetTest(::grpc::ClientContext* context, const ::Input& request, ::OutPut* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::OutPut>> AsyncGetTest(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::OutPut>>(AsyncGetTestRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::OutPut>> PrepareAsyncGetTest(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::OutPut>>(PrepareAsyncGetTestRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderWriter< ::Input, ::OutPut>> GetStreamTest(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriter< ::Input, ::OutPut>>(GetStreamTestRaw(context));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>> AsyncGetStreamTest(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>>(AsyncGetStreamTestRaw(context, cq, tag));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>> PrepareAsyncGetStreamTest(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>>(PrepareAsyncGetStreamTestRaw(context, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void GetTest(::grpc::ClientContext* context, const ::Input* request, ::OutPut* response, std::function<void(::grpc::Status)>) override;
      void GetTest(::grpc::ClientContext* context, const ::Input* request, ::OutPut* response, ::grpc::ClientUnaryReactor* reactor) override;
      void GetStreamTest(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::Input,::OutPut>* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::OutPut>* AsyncGetTestRaw(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::OutPut>* PrepareAsyncGetTestRaw(::grpc::ClientContext* context, const ::Input& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReaderWriter< ::Input, ::OutPut>* GetStreamTestRaw(::grpc::ClientContext* context) override;
    ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>* AsyncGetStreamTestRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReaderWriter< ::Input, ::OutPut>* PrepareAsyncGetStreamTestRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_GetTest_;
    const ::grpc::internal::RpcMethod rpcmethod_GetStreamTest_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status GetTest(::grpc::ServerContext* context, const ::Input* request, ::OutPut* response);
    virtual ::grpc::Status GetStreamTest(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* stream);
  };
  template <class BaseClass>
  class WithAsyncMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_GetTest() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetTest(::grpc::ServerContext* context, ::Input* request, ::grpc::ServerAsyncResponseWriter< ::OutPut>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_GetStreamTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_GetStreamTest() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_GetStreamTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetStreamTest(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetStreamTest(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::OutPut, ::Input>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(1, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_GetTest<WithAsyncMethod_GetStreamTest<Service > > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_GetTest() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::Input, ::OutPut>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::Input* request, ::OutPut* response) { return this->GetTest(context, request, response); }));}
    void SetMessageAllocatorFor_GetTest(
        ::grpc::MessageAllocator< ::Input, ::OutPut>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::Input, ::OutPut>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* GetTest(
      ::grpc::CallbackServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithCallbackMethod_GetStreamTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_GetStreamTest() {
      ::grpc::Service::MarkMethodCallback(1,
          new ::grpc::internal::CallbackBidiHandler< ::Input, ::OutPut>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->GetStreamTest(context); }));
    }
    ~WithCallbackMethod_GetStreamTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetStreamTest(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::Input, ::OutPut>* GetStreamTest(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  typedef WithCallbackMethod_GetTest<WithCallbackMethod_GetStreamTest<Service > > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_GetTest() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_GetStreamTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_GetStreamTest() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_GetStreamTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetStreamTest(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_GetTest() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetTest(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_GetStreamTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_GetStreamTest() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_GetStreamTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetStreamTest(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetStreamTest(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(1, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_GetTest() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->GetTest(context, request, response); }));
    }
    ~WithRawCallbackMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* GetTest(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_GetStreamTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_GetStreamTest() {
      ::grpc::Service::MarkMethodRawCallback(1,
          new ::grpc::internal::CallbackBidiHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->GetStreamTest(context); }));
    }
    ~WithRawCallbackMethod_GetStreamTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetStreamTest(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::OutPut, ::Input>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* GetStreamTest(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetTest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_GetTest() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::Input, ::OutPut>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::Input, ::OutPut>* streamer) {
                       return this->StreamedGetTest(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_GetTest() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetTest(::grpc::ServerContext* /*context*/, const ::Input* /*request*/, ::OutPut* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetTest(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::Input,::OutPut>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_GetTest<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_GetTest<Service > StreamedService;
};


#endif  // GRPC_test_2eproto__INCLUDED
