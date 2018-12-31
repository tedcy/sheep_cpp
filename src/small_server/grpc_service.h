#pragma once
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "grpc_server.h"
#include "small_packages.h"
#include <atomic>

#include "log.h"
#include "small_client.h"
#include "grpc_client.h"

namespace small_server{
template<typename ReqT, typename RespT, 
    typename AsyncServiceT, typename Task>
class GrpcService: public GrpcServiceI {
public:
    GrpcService(): 
        service_(std::make_shared<AsyncServiceT>()){
    }
    grpc::Service* GetAsyncService() override {
        return service_.get();
    }
    std::shared_ptr<GrpcServiceCtxI> CreateCtx(
            GrpcCoreCtx *ctx) override {
        if (!onMessageFunc_) {
            LOG(FATAL) << "OnMessage doesn't set";
            return nullptr;
        }
        return std::make_shared<GrpcServiceCtx>(
                service_, cq_, ctx, onMessageFunc_);
    }
    void SetCompletionQueue(
            std::shared_ptr<grpc::ServerCompletionQueue> cq) override {
        cq_ = cq;
    }
private:
    std::shared_ptr<AsyncServiceT> service_;
    std::shared_ptr<grpc::ServerCompletionQueue> cq_;
public:
class GrpcServiceCtx;
using OnMessageFunc = 
    std::function<void(GrpcServiceCtx &ctx, std::string &errMsg)>;
    void SetOnMessage(const OnMessageFunc &func) {
        onMessageFunc_ = func;
    }
    OnMessageFunc onMessageFunc_;
class GrpcServiceCtx: public GrpcServiceCtxI, 
    public std::enable_shared_from_this<GrpcServiceCtx> {
public:
    GrpcServiceCtx(std::shared_ptr<AsyncServiceT> service, 
            std::shared_ptr<grpc::ServerCompletionQueue> cq, 
            GrpcCoreCtx *coreCtx,
            const OnMessageFunc &func): 
        service_(service), 
        cq_(cq), ctx_(), responder_(&ctx_), status_(PROCESS),
        coreCtx_(coreCtx),
        lock_(small_lock::MakeRecursiveLock()),
        onMessageFunc_(func) ,
        task_(std::make_shared<Task>()){
        LOG(DEBUG) << "CREATE " << GetTraceId();
        service_->RequestHandler(&ctx_, 
                &req_, &responder_, cq_.get(), cq_.get(), coreCtx_);
    }
    GrpcServiceCtx(const GrpcServiceCtx&) = delete;
    GrpcServiceCtx& operator=(const GrpcServiceCtx&) = delete;
    //~GrpcServiceCtx() {
    //    LOG(DEBUG) << "~GrpcServiceCtx";
    //}
    void Init() override {
        myself_ = this->shared_from_this();
    }
    void Proceed(bool ok) override{
        auto lock = lock_;
        small_lock::UniqueGuard uniqueLock (lock);
        switch (status_) {
        case PROCESS: {
            LOG(DEBUG) << "PROCESS " << GetTraceId();
            status_ = FINISH;
            std::string errMsg;
            if (!ok) {
                errMsg = "canceled";
            }
            onMessageFunc_(*this, errMsg);
            break;
        }
        case FINISH: {
            LOG(DEBUG) << "FINISH " << GetTraceId();
            coreCtx_->Clean();
            myself_ = nullptr;
            break;
        }
        }
        //fix 不可以在这里加任何访问成员的代码，会导致core
        //这个函数尽量只在状态机内完成功能
    }
    void Finish() {
        auto status = grpc::Status::OK;
        status_ = FINISH;
        responder_.Finish(resp_, status, coreCtx_);
    }
    std::shared_ptr<small_lock::LockI> GetLock() {
        return lock_;
    }
    std::shared_ptr<Task> GetTask() {
        return task_; 
    }
    std::string GetTraceId() {
        return coreCtx_->GetTraceId();
    }
    std::shared_ptr<small_client::HttpClientWithService<GrpcServiceCtx>> GetHttpClient(
            small_client::ClientChannel &channel,
            const std::string &method, const std::string &host,
            const std::string &target,const std::string &req) {
        auto httpClient = std::make_shared<small_client::HttpClientWithService<GrpcServiceCtx>>(
                channel, method, host, target, req);
        httpClient->SetServiceCtx(myself_);
        return httpClient;
    }
    template <typename ClientReqT, typename ClientRespT,
             typename ClientStub>
    std::shared_ptr<GrpcClientWithService<
    ClientReqT, ClientRespT, ClientStub, GrpcServiceCtx>> 
        GetGrpcClient() {
        auto grpcClient = 
            std::make_shared<GrpcClientWithService<ClientReqT, ClientRespT,
            ClientStub, GrpcServiceCtx>>();
        grpcClient->Init();
        grpcClient->SetServiceCtx(myself_);
        return grpcClient;
    }
    ReqT req_;
    RespT resp_;
private:
    std::shared_ptr<AsyncServiceT> service_;
    std::shared_ptr<grpc::ServerCompletionQueue> cq_;
    grpc::ServerContext ctx_;
    grpc::ServerAsyncResponseWriter<RespT> responder_;
    enum Status {
        PROCESS, 
        FINISH 
    };
    Status status_;
    OnMessageFunc onMessageFunc_;
    GrpcCoreCtx *coreCtx_ = nullptr;
    //composition
    std::shared_ptr<small_lock::LockI> lock_;
    std::shared_ptr<GrpcServiceCtx> myself_;
    std::shared_ptr<Task> task_;
};
};
} //namespace search
