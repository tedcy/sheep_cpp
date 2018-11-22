#pragma once
#include <functional>
#include <string>
#include <grpcpp/grpcpp.h>

#include "grpc_core.h"

namespace small_server{
template <typename ReqT, typename RespT, typename Stub>
class GrpcClient;

template <typename ReqT, typename RespT, typename Stub>
struct CallBack{
    virtual void OnDone(GrpcClient<ReqT, RespT, Stub> &c, const std::string &errMsg) = 0;
    virtual ~CallBack() = default;
};
template <typename ReqT, typename RespT, typename Stub, typename T>
struct CallBackT: public CallBack<ReqT, RespT, Stub>{
    void OnDone(GrpcClient<ReqT, RespT, Stub> &c, const std::string &errMsg) override {
        T &t = dynamic_cast<T&>(c);
        callback_(t, errMsg);
    }
    std::function<void(T&, const std::string &errMsg)> callback_;
};

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
        onDone_->OnDone(*this, errMsg);
        ctx_->Clean();
    }
    ReqT req_;
    RespT resp_;
    void DoReq(const std::string &addr,
            std::function<void(GrpcClient&, const std::string&)> onDone) {
        doReq<GrpcClient>(addr, onDone);
    }
protected:
    template <typename T>
    void doReq(const std::string &addr,
            std::function<void(T&, const std::string&)> &onDone) {
        auto callback = std::make_shared<CallBackT<ReqT, RespT, Stub, T>>();
        callback->callback_ = onDone;
        onDone_ = callback;

        auto cq = GrpcCore::GetInstance()->GetCompletionQueue().get();
        //todo stub manager
        auto stub = Stub::NewStub(
            grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()));
        responseReader_ = stub->PrepareAsyncHandler(
            &grpcContext_, req_, cq);
        responseReader_->StartCall();
        responseReader_->Finish(&resp_, &status_, ctx_);
    }
private:
    grpc::Status status_;
    GrpcCoreCtx *ctx_ = nullptr;
    //composition
    std::shared_ptr<CallBack<ReqT, RespT, Stub>> onDone_;
    std::unique_ptr<grpc::ClientAsyncResponseReader<RespT>>
        responseReader_;
    grpc::ClientContext grpcContext_;
};
template <typename ReqT, typename RespT, typename Stub, typename ServiceCtxT>
class GrpcClientWithService: public GrpcClient<ReqT, RespT, Stub>{
public:
    void DoReq(const std::string &addr,
            std::function<void(GrpcClientWithService&, const std::string&)> onDone) {
        this->template doReq<GrpcClientWithService>(addr, onDone);
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
