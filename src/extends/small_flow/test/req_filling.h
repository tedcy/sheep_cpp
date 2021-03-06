#pragma once
#include "small_flow.h"
#include "service.h"
#include "small_packages.h"

namespace test_service {
namespace ploy{
class ReqFilling: public small_flow::Ploy<GrpcServiceWithTestEvent>,
    public small_packages::noncopyable {
public:
    void Run(GrpcServiceWithTestEvent &event, void *req, void *resp) override {
        Req *realReq = static_cast<Req*>(req);
        realReq->name = event.req_.name();
    }
};
}
}
