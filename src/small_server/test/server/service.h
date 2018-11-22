#include "small_server.h"
#include "helloworld.grpc.pb.h"
#include "helloworld.pb.h"

namespace test_service {
class TestService;
struct TestTask;
using GrpcServiceWithTest = small_server::GrpcService<
    helloworld::HelloRequest, helloworld::HelloReply,
    helloworld::Greeter::AsyncService, TestTask>;
using GrpcServiceWithTestCtx = GrpcServiceWithTest::GrpcServiceCtx;
using GrpcServiceClientTest = 
    small_server::GrpcClientWithService<helloworld::HelloRequest,
        helloworld::HelloReply, helloworld::Greeter, GrpcServiceWithTestCtx>;
struct TestTask {
    TestTask() = default;
    TestTask(const TestTask&) = delete;
    TestTask& operator=(const TestTask&) = delete;
    std::shared_ptr<GrpcServiceClientTest> client;
};
class TestService: public GrpcServiceWithTest{
public:
    void Init() {
        auto onMessage = [this](GrpcServiceWithTestCtx &ctx,
            std::string &errMsg) {
            auto name = ctx.req_.name();
            if (name == "proxy") {
                auto client = ctx.GetGrpcClient<helloworld::HelloRequest,
                    helloworld::HelloReply, helloworld::Greeter>();
                ctx.GetTask()->client = client;
                client->req_.set_name("server");
                client->DoReq("127.0.0.1:8888", 
                    [](GrpcServiceClientTest &client, const std::string &errMsg) {
                        auto serviceCtx = client.GetServiceCtx().lock();
                        if (!serviceCtx) {
                            return;
                        }
                        serviceCtx->resp_.set_message(client.resp_.message());
                        serviceCtx->Finish();
                    });
                return;
            }else {
                auto task = ctx.GetTask();
                ctx.resp_.set_message("from server");
                ctx.Finish();
                return;
            }
        };
        SetOnMessage(onMessage);
    }
    std::string Name() override {
        return "test";
    }
};
}
