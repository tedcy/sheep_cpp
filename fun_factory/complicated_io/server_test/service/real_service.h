#pragma once
#include "../../server/service/service.h"
#include "../../server/event_loop/event_loop.h"

class RealService:public Service {
    enum class RealServiceStage {
        Init,
        ParseCmd,
        RecvReq,
        SendResp,
        Finish
    };
    public:
        RealService() = default;
        ~RealService() = default;
    private:
        void Handler() override;
        void EndHandler() override;
        RealServiceStage stage_ = RealServiceStage::Init;
        std::shared_ptr<BufferWrapper> buffer_;
};
