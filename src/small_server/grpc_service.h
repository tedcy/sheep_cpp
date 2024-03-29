#pragma once
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "grpc_server.h"
#include "extends/small_packages/small_packages.h"
#include <atomic>

#include "log/log.h"
#include "small_client/small_client.h"
#include "small_server/grpc_client.h"

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
    std::shared_ptr<GrpcServiceEventI> CreateEvent(
            GrpcEvent *event) override {
        if (!onMessageFunc_) {
            LOG(FATAL) << "OnMessage doesn't set";
            return nullptr;
        }
        return std::make_shared<GrpcServiceEvent>(
                service_, cq_, event, onMessageFunc_);
    }
    void SetCompletionQueue(
            std::shared_ptr<grpc::ServerCompletionQueue> cq) override {
        cq_ = cq;
    }
private:
    std::shared_ptr<AsyncServiceT> service_;
    std::shared_ptr<grpc::ServerCompletionQueue> cq_;
public:
class GrpcServiceEvent;
using OnMessageFunc = 
    std::function<void(GrpcServiceEvent &event, std::string &errMsg)>;
    void SetOnMessage(const OnMessageFunc &func) {
        onMessageFunc_ = func;
    }
    OnMessageFunc onMessageFunc_;
class GrpcServiceEvent: public GrpcServiceEventI, 
    public std::enable_shared_from_this<GrpcServiceEvent>,
    public small_packages::noncopyable {
public:
    GrpcServiceEvent(std::shared_ptr<AsyncServiceT> service, 
            std::shared_ptr<grpc::ServerCompletionQueue> cq, 
            GrpcEvent *looperEvent,
            const OnMessageFunc &func): 
        service_(service), 
        cq_(cq), event_(), responder_(&event_), status_(PROCESS),
        looperEvent_(looperEvent),
        lock_(small_lock::MakeRecursiveLock()),
        onMessageFunc_(func) ,
        task_(std::make_shared<Task>()){
        LOG(DEBUG) << "CREATE " << GetTraceId();
        service_->RequestHandler(&event_, 
                &req_, &responder_, cq_.get(), cq_.get(), looperEvent_);
    }
    void Init() override {
        myself_ = this->shared_from_this();
    }
    void Proceed(bool ok) override{
        small_lock::UniqueGuard uniqueLock (lock_);
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
            looperEvent_->Clean();
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
        responder_.Finish(resp_, status, looperEvent_);
    }
    std::shared_ptr<small_lock::LockI> GetLock() {
        return lock_;
    }
    std::shared_ptr<Task> GetTask() {
        return task_; 
    }
    std::string GetTraceId() {
        return looperEvent_->GetTraceId();
    }
    std::shared_ptr<small_client::HttpClientWithService<GrpcServiceEvent>> GetHttpClient(
            small_client::ClientChannel &channel,
            const std::string &method,
            const std::string &target,const std::string &req) {
        auto httpClient = std::make_shared<small_client::HttpClientWithService<GrpcServiceEvent>>(
                channel, method, target, req);
        httpClient->SetServiceEvent(myself_);
        return httpClient;
    }
    std::shared_ptr<small_client::RedisClientWithService<GrpcServiceEvent>> GetRedisClient(
            small_client::ClientChannel &channel) {
        auto redisClient = std::make_shared<small_client::RedisClientWithService<GrpcServiceEvent>>(
                channel);
        redisClient->SetServiceEvent(myself_);
        return redisClient;
    }
    template <typename ClientReqT, typename ClientRespT,
             typename ClientStub>
    std::shared_ptr<GrpcClientWithService<
    ClientReqT, ClientRespT, ClientStub, GrpcServiceEvent>> 
        GetGrpcClient(GrpcClientChannel<ClientStub> &channel) {
        auto grpcClient = 
            std::make_shared<GrpcClientWithService<ClientReqT, ClientRespT,
            ClientStub, GrpcServiceEvent>>(channel);
        grpcClient->SetServiceEvent(myself_);
        return grpcClient;
    }
    ReqT req_;
    RespT resp_;
private:
    std::shared_ptr<AsyncServiceT> service_;
    std::shared_ptr<grpc::ServerCompletionQueue> cq_;
    grpc::ServerContext event_;
    grpc::ServerAsyncResponseWriter<RespT> responder_;
    enum Status {
        PROCESS, 
        FINISH 
    };
    Status status_;
    OnMessageFunc onMessageFunc_;
    GrpcEvent *looperEvent_ = nullptr;
    //composition
    std::shared_ptr<small_lock::LockI> lock_;
    std::shared_ptr<GrpcServiceEvent> myself_;
    std::shared_ptr<Task> task_;
};
};
} //namespace search
