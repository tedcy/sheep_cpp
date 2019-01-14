#pragma once
#include "log.h"
#include "small_packages.h"
#include "trace_id.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

namespace small_server{
class GrpcEventI {
public:
    virtual ~GrpcEventI() = default;
    virtual void Proceed(bool ok) = 0;
};
class GrpcEvent {
public:
    explicit GrpcEvent(std::shared_ptr<GrpcEventI> event) :
        traceId_(TraceInstance::Get()->GetTraceId()),
        event_(event) {
    }
    void Proceed(bool ok) {
        event_->Proceed(ok);
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
    std::shared_ptr<GrpcEventI> event_;
};
class GrpcLooper: public small_packages::noncopyable{
public:
    static GrpcLooper* GetInstance() {
        static GrpcLooper instance;
        return &instance;
    }
    ~GrpcLooper() {
        Shutdown();
    }
    void Init(grpc::ServerBuilder &builder) {
        if (scq_) {
            LOG(FATAL) << "GrpcLooper has inited";
            return;
        }
        scq_ = builder.AddCompletionQueue();
        cq_ = scq_;
        run();
    }
    void Init() {
        if (cq_) {
            LOG(FATAL) << "GrpcLooper has inited";
            return;
        }
        cq_ = std::make_shared<grpc::CompletionQueue>();
        run();
    }
    std::shared_ptr<grpc::CompletionQueue> 
        GetCompletionQueue() {
        return cq_;
    }
    std::shared_ptr<grpc::ServerCompletionQueue> 
        GetServerCompletionQueue() {
        return scq_;
    }
    void Shutdown() {
        if (thread_ != nullptr) {
            stop_ = true;
            thread_->join();
        }
    }
private:
    void run() {
        thread_ = std::unique_ptr<std::thread>(
        new std::thread([this]() {
            void *tag;
            bool ok;
            while(!stop_) {
                cq_->Next(&tag, &ok);
                static_cast<GrpcEvent*>(tag)->Proceed(ok);
            }
        }));
    }
    std::atomic_bool stop_ = {false};
    std::shared_ptr<grpc::CompletionQueue> cq_;
    std::shared_ptr<grpc::ServerCompletionQueue> scq_;
    std::unique_ptr<std::thread> thread_;
};
}//namespace small_server
