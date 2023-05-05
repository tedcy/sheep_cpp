#pragma once
#include "small_flow.h"
#include "service.h"
#include "small_packages.h"

namespace test_service {
namespace ploy{
class RespFilling: public small_flow::Ploy<GrpcServiceWithTestEvent>,
    public small_packages::noncopyable {
public:
    void Run(GrpcServiceWithTestEvent &event, void *req, void *resp) override {
        Resp *realResp = static_cast<Resp*>(resp);
        event.resp_.set_message(realResp->name);
    }
};
}
}
