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
class HttpClient: public small_packages::noncopyable{
public:
using HttpClientOnDone = 
    std::function<void(HttpClient& c, const std::string &errMsg)>;
    HttpClient(const std::string &method,
            const std::string &host,const std::string &port,
            const std::string &target,const std::string &req) {
        client_ = std::make_shared<small_http_client::Async>(
                method, host, port, target, req);
    }
    //~HttpClient() {
    //    LOG(DEBUG) << "~HttpClient";
    //}
    void DoReq(HttpClientOnDone onDone) {
        doReq<HttpClient>(onDone);
    }
    std::string GetRespStr() {
        return respStr_;
    }
    std::shared_ptr<SmallMap> headers_;
    std::shared_ptr<SmallMap> queryStrings_;
    uint64_t readTimeout_ = 0;
    uint64_t writeTimeout_ = 0;
    std::string traceId_;
protected:
    std::string respStr_;
    template <typename T>
    void doReq(std::function<void(T&, const std::string&)> &onDone) {
        auto realOnDone = [onDone](HttpClient &c, const std::string &errMsg) {
            T &t = dynamic_cast<T&>(c);
            onDone(t, errMsg);
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
        auto smallHttpLibOnDone = [this, realOnDone](const std::string &respStr, const std::string &errMsg) {
            respStr_ = respStr;
            realOnDone(*this, errMsg);
        };
        client_->doReq(smallHttpLibOnDone);
    }
private:
    //composition
    std::shared_ptr<small_http_client::Async> client_;
};
template<typename ServiceCtxT>
class HttpClientWithService: public HttpClient{
public:
    HttpClientWithService(const std::string &method,
            const std::string &host,const std::string &port,
            const std::string &target,const std::string &req) :
        HttpClient(method, host, port, target, req) {
    }
    void DoReq(std::function<void(HttpClientWithService&, const std::string&)> onDone) {
        //FIXME: auto realOnDone lead to be failed, wtf?
        std::function<void(HttpClientWithService&, const std::string&)> realOnDone;
        realOnDone = [this, onDone](HttpClientWithService&, const std::string &errMsg) {
            auto realCtx = serviceCtx_.lock();
            if (!realCtx) {
                return;
            }
            auto lock = realCtx->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            onDone(*this, errMsg);
        };
        this->template doReq<HttpClientWithService>(realOnDone);
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
