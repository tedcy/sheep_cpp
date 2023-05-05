#pragma once
#include "small_server.h"
#include "small_flow/protos/helloworld.grpc.pb.h"
#include "small_flow/protos/helloworld.pb.h"
#include "small_packages.h"
#include "small_flow.h"

extern small_server::GrpcClientChannel<helloworld::Greeter> channel;

namespace test_service {
class TestService;
struct TestTask;
using GrpcServiceWithTest = small_server::GrpcService<
    helloworld::HelloRequest, helloworld::HelloReply,
    helloworld::Greeter::AsyncService, TestTask>;
using GrpcServiceWithTestEvent = GrpcServiceWithTest::GrpcServiceEvent;
using GrpcServiceClientTest = 
    small_server::GrpcClientWithService<helloworld::HelloRequest,
        helloworld::HelloReply, helloworld::Greeter, GrpcServiceWithTestEvent>;
struct Req{
    std::string name;
};
struct Resp{
    std::string name;
};
struct TestTask: public small_packages::noncopyable {
    small_flow::FlowCtx ctx_;
    Req req_;
    Resp resp_;
    TestService *service_;
    std::shared_ptr<GrpcServiceClientTest> client_;
};
class TestService: public GrpcServiceWithTest,
    public small_flow::FlowService<GrpcServiceWithTestEvent>{
public:
    void Init() {
        auto onMessage = [this](GrpcServiceWithTestEvent &event,
            std::string &errMsg) {
            auto task = event.GetTask();
            task->service_ = this;
            task->ctx_.traceId = event.GetTraceId();
            task->ctx_.errMsg = errMsg;
            Run(event, &task->req_, &task->resp_);
        };
        SetOnMessage(onMessage);
    }
    std::string Name() override {
        return "test";
    }
};
}
