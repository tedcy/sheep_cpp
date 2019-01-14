#pragma once
#include "log.h"
#include "small_packages.h"
#include "trace_id.h"
#include <string>
#include <memory>
#include <thread>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

namespace small_server{
class GrpcCoreCtxI {
public:
    virtual ~GrpcCoreCtxI() = default;
    virtual void Proceed(bool ok) = 0;
};
class GrpcCoreCtx {
public:
    explicit GrpcCoreCtx(std::shared_ptr<GrpcCoreCtxI> ctx) :
        traceId_(TraceInstance::Get()->GetTraceId()),
        ctx_(ctx) {
    }
    void Proceed(bool ok) {
        ctx_->Proceed(ok);
    }
    void Clean() {
        delete this;
    }
    std::string GetTraceId() {
        return traceId_;
    }
private:
    std::string traceId_;
    //composition
    std::shared_ptr<GrpcCoreCtxI> ctx_;
};
class GrpcLooper: public small_packages::noncopyable{
public:
    static GrpcLooper* GetInstance() {
        static GrpcLooper instance;
        return &instance;
    }
    void Init(grpc::ServerBuilder &builder) {
        if (scq_) {
            LOG(FATAL) << "GrpcLooper has inited";
            return;
        }
        scq_ = builder.AddCompletionQueue();
        cq_ = scq_;
    }
    void Init() {
        if (cq_) {
            LOG(FATAL) << "GrpcLooper has inited";
            return;
        }
        cq_ = std::make_shared<grpc::CompletionQueue>();
    }
    std::shared_ptr<grpc::CompletionQueue> 
        GetCompletionQueue() {
        return cq_;
    }
    std::shared_ptr<grpc::ServerCompletionQueue> 
        GetServerCompletionQueue() {
        return scq_;
    }
    void Run() {
        if (!cq_) {
            LOG(FATAL) << "GrpcLooper is not inited";
            return;
        }
        for (auto i = 0;i < 1;i++) {
            vs_.push_back(std::thread(
                [this]() {
                    void *tag;
                    bool ok;
                    while(1) {
                        cq_->Next(&tag, &ok);
                        static_cast<GrpcCoreCtx*>(tag)->Proceed(ok);
                    }
                }
            ));
        }
    }
    void Wait() {
        for (auto &v : vs_) {
            v.join();
        }
    }
private:
    std::shared_ptr<grpc::CompletionQueue> cq_;
    std::shared_ptr<grpc::ServerCompletionQueue> scq_;
    std::vector<std::thread> vs_;
};
}//namespace small_server
