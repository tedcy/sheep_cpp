#pragma once
#include <functional>
#include <string>
#include "small_http_client.h"
#include "small_packages.h"
#include "log.h"
namespace small_server{
class SmallMap {
public:
    SmallMap() = default;
    explicit SmallMap(const std::map<std::string, std::string> &kvs) :
        kvs_(kvs){}
    void Set(const std::string &key,const std::string &value){
        kvs_.insert({key, value});
    }
    std::map<std::string, std::string> kvs_;
};
template<typename ServiceCtxT>
class HttpClient {
public:
using HttpClientOnDone = 
    std::function<void(HttpClient& c, const std::string &errMsg)>;
    HttpClient(const std::string &method,
            const std::string &host,const std::string &port,
            const std::string &target,const std::string &req) {
        client_ = std::make_shared<small_http_client::Async>(
                method, host, port, target, req);
    }
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    //~HttpClient() {
    //    LOG(DEBUG) << "~HttpClient";
    //}
    void DoReq(HttpClientOnDone onDone) {
        auto realOnDone = [this, onDone](const std::string &respStr, const std::string &errMsg) {
            auto realCtx = serviceCtx_.lock();
            if (!realCtx) {
                return;
            }
            auto lock = realCtx->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            respStr_ = respStr;
            onDone(*this, errMsg);
        };
        if (headers_) {
            auto realHeader = std::make_shared<small_http_client::Headers>(headers_->kvs_);
            client_->setHeaders(realHeader);
        }
        if (queryStrings_) {
            auto realQueryString = std::make_shared<small_http_client::QueryStrings>(queryStrings_->kvs_);
            client_->setQueryStrings(realQueryString);
        }
        if (readTimeout_ != 0) {
            client_->SetReadTimeout(readTimeout_);
        }
        if (writeTimeout_ != 0) {
            client_->SetWriteTimeout(writeTimeout_);
        }
        client_->doReq(realOnDone);
    }
    std::weak_ptr<ServiceCtxT> GetServiceCtx() {
        return serviceCtx_;
    }
    std::string GetRespStr() {
        return respStr_;
    }
    std::shared_ptr<SmallMap> headers_;
    std::shared_ptr<SmallMap> queryStrings_;
    uint64_t readTimeout_ = 0;
    uint64_t writeTimeout_ = 0;
    std::string traceId_;
private:
    friend ServiceCtxT;
    void SetServiceCtx(std::shared_ptr<ServiceCtxT> serviceCtx) {
        serviceCtx_ = serviceCtx;
    }
    std::string respStr_;
    //composition
    std::shared_ptr<small_http_client::Async> client_;
    //association
    std::weak_ptr<ServiceCtxT> serviceCtx_;
};
}//namespace small_server
