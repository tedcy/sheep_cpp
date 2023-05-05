#pragma once
#include "small_flow.h"
#include "service.h"
#include "small_packages.h"

namespace test_service {
namespace ploy{
class ReqForward: public small_flow::Ploy<GrpcServiceWithTestEvent>,
    public small_packages::noncopyable {
public:
    void Run(GrpcServiceWithTestEvent &event, void *req, void *resp) override {
        Req *realReq = static_cast<Req*>(req);
        Resp *realResp = static_cast<Resp*>(resp);

        auto task = event.GetTask();
        if (realReq->name == "proxy") {
            task->client_ = event.GetGrpcClient<helloworld::HelloRequest,
                helloworld::HelloReply, helloworld::Greeter>(channel);
        
            auto client = event.GetTask()->client_;
            client->req_.set_name("server");
            client->DoReq([](GrpcServiceClientTest &client, const std::string &errMsg) {
                auto serviceEvent = client.GetServiceEvent().lock();
                if (!serviceEvent) {
                    return;
                }
                auto task = serviceEvent->GetTask();
                task->resp_.name = client.resp_.message();
                task->service_->Run(*serviceEvent, &task->req_, &task->resp_);
            });
            task->ctx_.suspend = true;
        }else {
            task->resp_.name = "server";
        }
    }
};
}
}
