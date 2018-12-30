#pragma once
#include <functional>
#include <string>
#include "net.h"
#include "small_packages.h"
#include "log.h"
#include "net_client.h"
#include "small_http_parser.h"
namespace small_server{
class HttpClientBackUp : public NetClient{
public:
using HttpClientBackUpOnDone = 
    std::function<void(HttpClientBackUp& c, const std::string &errMsg)>;
    HttpClientBackUp(SheepNetClientCore &core, const std::string method,
            const std::string &host, const std::string &target, const std::string &body) :
        NetClient(core) ,
        formarter_(method, host, target, body){
    }
    void SetHeaders(const small_http_parser::Map &map) {
        formarter_.SetHeader(map);
    }
    void SetQueryString(const small_http_parser::Map &map) {
        formarter_.SetQueryString(map);
    }
    void DoReq(HttpClientBackUpOnDone onDone) {
        doReq<HttpClientBackUp>(onDone);
    }
    ~HttpClientBackUp() {
    }
    const std::string& GetRespStr() {
        return parser_.GetBody();
    }
    const small_http_parser::Map& GetRespHeader() {
        return parser_.GetHeader();
    }
private:
    void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) override {
        const std::string &req = formarter_.Format();
        connection.WriteBuffer_.Push(const_cast<char*>(req.c_str()), req.size());
    }
    void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) override{
        finish = false;
        for (;;) {
            char tmpBuf[1024];
            uint64_t len;
            len = connection.ReadBuffer_.PopHead(tmpBuf, 1024);
            if (len == 0) {
                break;
            }
            std::string tmp(tmpBuf, len);
            parser_.Feed(errMsg, finish, tmp);
        }
    }
    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    HttpClientBackUpOnDone onDone_;

    //req
    small_http_parser::ReqFormater formarter_;
    //resp
    small_http_parser::RespParser parser_;
};

template<typename ServiceCtxT>
class HttpClientBackUpWithService: public HttpClientBackUp{
public:
    HttpClientBackUpWithService(SheepNetClientCore &core, const std::string &method,
            const std::string &host,
            const std::string &target,const std::string &body) :
        HttpClientBackUp(core, method, host, target, body) {
    }
    void DoReq(std::function<void(HttpClientBackUpWithService&, const std::string&)> onDone) {
        auto realOnDone = [this, onDone](HttpClientBackUpWithService&, const std::string &errMsg) {
            auto realCtx = serviceCtx_.lock();
            if (!realCtx) {
                return;
            }
            auto lock = realCtx->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            onDone(*this, errMsg);
        };
        this->template doReq<HttpClientBackUpWithService>(realOnDone);
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
}
