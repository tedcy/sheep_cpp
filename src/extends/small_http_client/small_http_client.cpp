#include "small_http_client.h"
#include "connection.h"
#include "connection_pool.h"
#include "connection_pool_manager.h"

using std::string;
namespace http = boost::beast::http;

namespace small_http_client{
Async::Async(const std::string &method,const std::string &host,const std::string &port, 
            const std::string &target,const std::string &req):
    host_(host), port_(port), target_(target), req_(), resp_(), onDone_() {
    setMethod(method);
    req_.set(http::field::host, host);
    setReq(req);
    req_.target(target);
}

void Async::doReq(const std::function<void(const std::string&, const std::string&)> &onDone) {
    onDone_ = onDone;
    connectionPool_ = ConnectionPoolManager::getInstance()->get(host_, port_);
    if (connectionPool_ == nullptr) {
        onDone_("", "find connectionPool failed: " + host_ + ":" + port_);
        return;
    }
    connection_ = connectionPool_->get();
    if (connection_ == nullptr) {
        onDone_("", "find connection failed: " + host_ + ":" + port_);
        return;
    }
    std::function<void(const std::string&)> onWriteCB = [this](const std::string& errMsg) {
        onWrite(errMsg);
    };
    connection_->asyncWrite(req_, onWriteCB);
    return;
}

void Async::onWrite(const std::string &errMsg) {
    if (errMsg != "") {
        connectionPool_->put(connection_);
        onDone_("", errMsg);
        return;
    }
    std::function<void(const std::string &errMsg)> onReadCB = [this](const std::string& errMsg) {
        onRead(errMsg);
    };
    connection_->asyncRead(resp_, onReadCB);
}

void Async::onRead(const std::string &errMsg) {
    connectionPool_->put(connection_);
    if (errMsg != "") {
        onDone_("", errMsg);
        return;
    }
    onDone_(resp_.body(), "");
}

//default as get
//todo add all method
void Async::setMethod(const string &method) {
    if (method == "GET" || method == "get") {
        req_.method(http::verb::get);
        return;
    }
    if (method == "POST" || method == "post") {
        req_.method(http::verb::post);
        return;
    }
    if (method == "PUT" || method == "put") {
        req_.method(http::verb::put);
        return;
    }
    req_.method(http::verb::get);
}
void Async::setQueryStrings(std::shared_ptr<QueryStrings> queryStrings) {
    string realTarget = target_;
    if (queryStrings != nullptr) {
        int i = 0;
        for (auto &p : *queryStrings) {
            if (i == 0) {
                realTarget += "?";
            }else {
                realTarget += "&";
            }
            realTarget += (p.first + "=" + p.second);
            i++;
        }
    }
    req_.target(realTarget);
}
void Async::setHeaders(std::shared_ptr<Headers> headers) {
    if (headers == nullptr) {
        return;
    }
    for (auto &h : *headers) {
        req_.set(http::string_to_field(h.first), h.second);
    }
}
void Async::setReq(const string &req) {
    req_.body() = req;
    req_.set(http::field::content_length, req.size());
}
}
