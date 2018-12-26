#pragma once
#include <functional>
#include <string>
#include <grpcpp/grpcpp.h>

#include "grpc_core.h"
#include "grpc_client_core.h"

namespace small_server{
template <typename ReqT, typename RespT, typename Stub>
class GrpcClient: public GrpcCoreCtxI, 
    public std::enable_shared_from_this<GrpcClient<ReqT, RespT, Stub>>{
public:
    std::string GetTraceId() {
        return ctx_->GetTraceId();
    }
    GrpcClient() = default;
    GrpcClient(const GrpcClient&) = delete;
    GrpcClient& operator=(const GrpcClient&) = delete;
    void Init() {
        ctx_ = new GrpcCoreCtx(this->shared_from_this());
        LOG(DEBUG) << "CREATE " << GetTraceId();
    }
    void Proceed(bool ok) {
        LOG(DEBUG) << "FINISH " << GetTraceId();
        std::string errMsg;
        if (!ok || !status_.ok()) {
            errMsg = "Grpc Client failed";
        }
        onDone_(*this, errMsg);
        ctx_->Clean();
    }
    ReqT req_;
    RespT resp_;
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
        auto stub = GrpcClientCore<Stub>::GetInstance()->GetClientPool(ok);
        if (!ok) {
            onDone_(*this, "stub is nullptr");
            return;
        }
        auto cq = GrpcCore::GetInstance()->GetCompletionQueue().get();
        responseReader_ = stub->PrepareAsyncHandler(
            &grpcContext_, req_, cq);
        responseReader_->StartCall();
        responseReader_->Finish(&resp_, &status_, ctx_);
    }
private:
    grpc::Status status_;
    GrpcCoreCtx *ctx_ = nullptr;
    std::function<void(GrpcClient&, const std::string &errMsg)> onDone_;
    //composition
    std::unique_ptr<grpc::ClientAsyncResponseReader<RespT>>
        responseReader_;
    grpc::ClientContext grpcContext_;
};
template <typename ReqT, typename RespT, typename Stub, typename ServiceCtxT>
class GrpcClientWithService: public GrpcClient<ReqT, RespT, Stub>{
public:
    void DoReq(std::function<void(GrpcClientWithService&, const std::string&)> onDone) {
        this->template doReq<GrpcClientWithService>(onDone);
    }
    std::weak_ptr<ServiceCtxT> GetServiceCtx() {
        return serviceCtx_;
    }
private:
    friend ServiceCtxT;
    void SetServiceCtx(std::shared_ptr<ServiceCtxT> serviceCtx) {
        serviceCtx_ = serviceCtx;
    }
    //association
    std::weak_ptr<ServiceCtxT> serviceCtx_;
};
}//namespace small_server
