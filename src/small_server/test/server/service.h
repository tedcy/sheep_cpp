#include "small_server.h"
#include "helloworld.grpc.pb.h"
#include "helloworld.pb.h"

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
struct TestTask {
    TestTask() = default;
    TestTask(const TestTask&) = delete;
    TestTask& operator=(const TestTask&) = delete;
    std::shared_ptr<GrpcServiceClientTest> client;
};
class TestService: public GrpcServiceWithTest{
public:
    void Init() {
        auto onMessage = [this](GrpcServiceWithTestEvent &event,
            std::string &errMsg) {
            auto name = event.req_.name();
            if (name == "proxy") {
                auto client = event.GetGrpcClient<helloworld::HelloRequest,
                    helloworld::HelloReply, helloworld::Greeter>(channel);
                event.GetTask()->client = client;
                client->req_.set_name("server");
                client->DoReq([](GrpcServiceClientTest &client, const std::string &errMsg) {
                        auto serviceEvent = client.GetServiceEvent().lock();
                        if (!serviceEvent) {
                            return;
                        }
                        serviceEvent->resp_.set_message(client.resp_.message());
                        serviceEvent->Finish();
                    });
                return;
            }else {
                auto task = event.GetTask();
                event.resp_.set_message("from server");
                event.Finish();
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
