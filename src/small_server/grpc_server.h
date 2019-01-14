#pragma once
#include "grpc_looper.h"
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include <string>
#include <map>
#include <atomic>

namespace small_server{
class GrpcServerEvent;
class GrpcServiceEventI {
public:
    virtual ~GrpcServiceEventI() = default;
    virtual void Init() = 0;
    virtual void Proceed(bool ok) = 0;
};
class GrpcServiceI{
public:
    virtual ~GrpcServiceI() = default;
    virtual std::string Name() = 0;
    virtual grpc::Service* GetAsyncService() = 0;
    virtual std::shared_ptr<GrpcServiceEventI> 
        CreateEvent(GrpcEvent *event) = 0;
    virtual void SetCompletionQueue(
            std::shared_ptr<grpc::ServerCompletionQueue> cq) = 0;
};
//dispatch events to service call data
class GrpcServerEvent :public GrpcEventI{
public:
    explicit GrpcServerEvent(GrpcServiceI *service):
        status_(PROCESS), service_(service) {
    }
    void Init(GrpcEvent *event) {
        event_ = event;
        serviceEvent_ = service_->CreateEvent(event_);
        serviceEvent_->Init();
    }
    void Proceed(bool ok) override {
        switch (status_){
        case PROCESS: {
            status_ = PROCESSING;
            auto serverEvent = std::make_shared<GrpcServerEvent>(service_);
            auto looperEvent = new GrpcEvent(serverEvent);
            serverEvent->Init(looperEvent);
            serviceEvent_->Proceed(ok);
            break;
        }
        case PROCESSING: {
            serviceEvent_->Proceed(ok);
            break;
        }}
    }
private:
    GrpcEvent *event_ = nullptr;
    enum Status{PROCESS, PROCESSING};
    Status status_;
    GrpcServiceI *service_ = nullptr;
    //composition
    std::shared_ptr<GrpcServiceEventI> serviceEvent_;
};
class GrpcServer{
public:
    explicit GrpcServer(const std::string& addr):
        addr_(addr){
    }
    void Init() {
        builder_.AddListeningPort(addr_, 
                grpc::InsecureServerCredentials());
    }
    void Register(GrpcServiceI *service) {
        auto asyncService = service->GetAsyncService();
        builder_.RegisterService(asyncService);
        services_.insert({service->Name(), service});
    }
    void Run() {
        GrpcLooper::GetInstance()->Init(builder_);
        auto server_ = builder_.BuildAndStart();
        LOG(INFO) << "GrpcServer listening on " << addr_;

        for (auto &servicePair: services_) {
            servicePair.second->SetCompletionQueue(
                GrpcLooper::GetInstance()->GetServerCompletionQueue());
            auto serverEvent = std::make_shared<GrpcServerEvent>(
                        servicePair.second);
            auto looperEvent = new GrpcEvent(serverEvent);
            serverEvent->Init(looperEvent);
        }
        GrpcLooper::GetInstance()->Run();
        GrpcLooper::GetInstance()->Wait();
        server_->Wait();
    }
private:
    std::string addr_;
    grpc::ServerBuilder builder_;
    std::shared_ptr<grpc::Server> server_;
    std::map<std::string, GrpcServiceI*> services_;
};
}
