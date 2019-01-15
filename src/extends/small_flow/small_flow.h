#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <ostream>
#include "log.h"
#include "small_packages.h"

namespace small_flow{
struct FlowCtx : public small_packages::noncopyable {
    std::string traceId = "unknown";
    std::string errMsg = "";
    uint32_t nextIndex = 0;
    bool suspend = false;
};
inline std::ostream& operator<<(std::ostream &output,
            const FlowCtx &ctx) {
    return output << "trace_id: " << ctx.traceId 
        << " nextIndex: " << ctx.nextIndex;
}
template <typename ServiceEventT>
class Ploy {
public:
    virtual void Init(std::string &errMsg){};
    virtual void Run(ServiceEventT &event, void *req, void *resp) = 0;
    virtual void Trace(void *req,
            void *resp) {};
};
template <typename ServiceEventT>
class FlowService {
public:
    void AddPloy(Ploy<ServiceEventT> *ploy) {
        std::string errMsg;
        ploy->Init(errMsg);
        ploys.push_back(ploy);
    }
    void Run(ServiceEventT &event, void *req, void *resp) {
        auto &ctx = event.GetTask()->ctx_;
        if (ctx.suspend) {
            ctx.suspend = false;
        }
        if (!ctx.errMsg.empty()) {
            LOG(ERROR) << ctx << " errMsg: " << ctx.errMsg;
            event.Finish();
            return;
        }
        LOG(DEBUG) << "FlowRUN " << ctx;
        for (int i = ctx.nextIndex;i < ploys.size();i++) {
            ctx.nextIndex = i + 1;
            ploys[i]->Run(event, req, resp);
            ploys[i]->Trace(req, resp);
            if (!ctx.errMsg.empty()) {
                LOG(ERROR) << ctx << " errMsg: " << ctx.errMsg;
                event.Finish();
                return;
            }
            if (ctx.suspend) {
                LOG(DEBUG) << "FlowSUSPEND " << ctx;
                return;
            }
        }
        LOG(DEBUG) << "FlowEND " << ctx;
        event.Finish();
    }
private:
    std::vector<Ploy<ServiceEventT>*> ploys;
};
} //namespace small_flow
