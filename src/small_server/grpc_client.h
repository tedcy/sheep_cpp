#pragma once
#include <functional>
#include <string>
#include <grpcpp/grpcpp.h>
#include "extends/small_packages/small_packages.h"

#include "grpc_looper.h"
#include "grpc_client_channel.h"

namespace small_server{
template <typename ReqT, typename RespT, typename Stub>
class GrpcClient: public GrpcEventI, 
    public std::enable_shared_from_this<GrpcClient<ReqT, RespT, Stub>>,
    public small_packages::noncopyable{
public:
    GrpcClient(GrpcClientChannel<Stub> &channel) :
        channel_(channel) {
    }
    std::string GetTraceId() {
        return event_->GetTraceId();
    }
    void Proceed(bool ok) {
        LOG(DEBUG) << "FINISH " << GetTraceId();
        std::string errMsg;
        if (!ok || !status_.ok()) {
            errMsg = "Grpc Client failed";
        }
        onDone_(*this, errMsg);
        event_->Clean();
    }
    ReqT req_;
    RespT resp_;
    void SetTimeoutMs(uint64_t ms) {
        std::chrono::system_clock::time_point deadline =
                std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
        grpcContext_.set_deadline(deadline);
    }
    void DoReq(std::function<void(GrpcClient&, const std::string&)> onDone) {
        doReq<GrpcClient>(onDone);
    }
protected:
    template <typename T>
    void doReq(std::function<void(T&, const std::string&)> &onDone) {
        onDone_ = [onDone](GrpcClient &c, const std::string &errMsg) {
            T &t = dynamic_cast<T&>(c);
            onDone(t, errMsg);
        };

        bool ok;
        auto stub = channel_.GetClientPool(ok);
        if (!ok) {
            onDone_(*this, "stub is nullptr");
            return;
        }
        event_ = new GrpcEvent(this->shared_from_this());
        LOG(DEBUG) << "CREATE " << GetTraceId();
        auto cq = GrpcLooper::GetInstance()->GetCompletionQueue().get();
        responseReader_ = stub->PrepareAsyncHandler(
            &grpcContext_, req_, cq);
        responseReader_->StartCall();
        responseReader_->Finish(&resp_, &status_, event_);
    }
private:
    grpc::Status status_;
    GrpcClientChannel<Stub> &channel_;
    GrpcEvent *event_ = nullptr;
    std::function<void(GrpcClient&, const std::string &errMsg)> onDone_;
    //composition
    std::unique_ptr<grpc::ClientAsyncResponseReader<RespT>>
        responseReader_;
    grpc::ClientContext grpcContext_;
};
template <typename ReqT, typename RespT, typename Stub, typename ServiceEventT>
class GrpcClientWithService: public GrpcClient<ReqT, RespT, Stub>{
public:
    GrpcClientWithService(GrpcClientChannel<Stub> &channel) :
        GrpcClient<ReqT, RespT, Stub>(channel){
    }
    void DoReq(std::function<void(GrpcClientWithService&, const std::string&)> onDone) {
        std::function<void(GrpcClientWithService&, const std::string&)> realOnDone;
        realOnDone = [this, onDone](GrpcClientWithService&, const std::string &errMsg) {
            auto realEvent = serviceEvent_.lock();
            if (!realEvent) {
                return;
            }
            auto lock = realEvent->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            onDone(*this, errMsg);
        };
        this->template doReq<GrpcClientWithService>(realOnDone);
    }
    std::weak_ptr<ServiceEventT> GetServiceEvent() {
        return serviceEvent_;
    }
private:
    friend ServiceEventT;
    void SetServiceEvent(std::shared_ptr<ServiceEventT> serviceEvent) {
        serviceEvent_ = serviceEvent;
    }
    //association
    std::weak_ptr<ServiceEventT> serviceEvent_;
};
}//namespace small_server
