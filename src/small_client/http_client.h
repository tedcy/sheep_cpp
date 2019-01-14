#pragma once
#include <functional>
#include <string>
#include "net.h"
#include "small_packages.h"
#include "log.h"
#include "base_client.h"
#include "small_http_parser.h"
namespace small_client{
class HttpClient : public BaseClient{
public:
using HttpClientOnDone = 
    std::function<void(HttpClient& c, const std::string &errMsg)>;
    HttpClient(ClientChannel &channel, const std::string method,
            const std::string &target, const std::string &body) :
        BaseClient(channel) ,
        formarter_(method, target, body){
    }
    void SetHeaders(const small_http_parser::Map &map) {
        formarter_.SetHeader(map);
    }
    void SetQueryString(const small_http_parser::Map &map) {
        formarter_.SetQueryString(map);
    }
    void SetHost(const std::string &host) {
        host_ = host;
    }
    void DoReq(HttpClientOnDone onDone) {
        doReq<HttpClient>(onDone);
    }
    ~HttpClient() {
    }
    const std::string& GetRespStr() {
        return parser_.GetBody();
    }
    const small_http_parser::Map& GetRespHeader() {
        return parser_.GetHeader();
    }
private:
    void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) override {
        if (host_.empty()) {
            host_ = GetAddr();
        }
        formarter_.SetHost(host_);
        const std::string &req = formarter_.Format();
        connection.WriteBufferPush(req.c_str(), req.size());
    }
    void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) override{
        finish = false;
        for (;;) {
            char tmpBuf[1024];
            uint64_t len;
            len = connection.ReadBufferPopHead(tmpBuf, 1024);
            if (len == 0) {
                break;
            }
            std::string tmp(tmpBuf, len);
            parser_.Feed(errMsg, finish, tmp);
            if (!errMsg.empty()) {
                errMsg += " with buffer: " + tmp;
                return;
            }
        }
    }
    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    HttpClientOnDone onDone_;
    std::string host_;

    //req
    small_http_parser::ReqFormater formarter_;
    //resp
    small_http_parser::RespParser parser_;
};

template<typename ServiceEventT>
class HttpClientWithService: public HttpClient{
public:
    HttpClientWithService(ClientChannel &channel, const std::string &method,
            const std::string &target,const std::string &body) :
        HttpClient(channel, method, target, body) {
    }
    void DoReq(std::function<void(HttpClientWithService&, const std::string&)> onDone) {
        //FIXME: auto is invliad, wtf need this line???
        std::function<void(HttpClientWithService&, const std::string&)> realOnDone;
        realOnDone = [this, onDone](HttpClientWithService&, const std::string &errMsg) {
            auto realEvent = serviceEvent_.lock();
            if (!realEvent) {
                return;
            }
            auto lock = realEvent->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            onDone(*this, errMsg);
        };
        this->doReq<HttpClientWithService>(realOnDone);
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
}
