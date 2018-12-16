#pragma once
#include <boost/beast/http.hpp>

#include <json/json.h>

#include <string>
#include <map>
#include <functional>
#include <memory>

namespace small_http_client {
typedef std::map<std::string,std::string> QueryStrings;
typedef std::map<std::string,std::string> Headers;

class Connection;
class ConnectionPool;

//todo 智能指针被释放立刻提示canceled
class Async: public std::enable_shared_from_this<Async> {
using onDoneHandler = std::function<void(const std::string&, 
    const std::shared_ptr<std::map<std::string,std::string>> respHeaders,
    const std::string&)>;
public:
    Async(const std::string &method,const std::string &host,const std::string &port, 
            const std::string &target,const std::string &req);
    ~Async();
    void SetOnDone(const std::function<void(const std::string&, const std::string&)> &onDone);
    void SetReadTimeout(uint32_t readTimeout);
    void SetWriteTimeout(uint32_t writeTimeout);
    void setQueryStrings(std::shared_ptr<QueryStrings> queryStrings);
    void setHeaders(std::shared_ptr<Headers> headers);
    void doReq(const onDoneHandler &onDone);
    void SetTraceId(const std::string &traceId);
private:
    void onRead(const std::string &resp, 
        const std::shared_ptr<std::map<std::string, std::string>> respHeaders,
        const std::string &errMsg);
    void setMethod(const std::string &method);
    void setReq(const std::string &req);
    std::shared_ptr<Connection> connection_ = nullptr;
    std::shared_ptr<ConnectionPool> connectionPool_ = nullptr;
    const std::string host_;
    const std::string port_;
    const std::string target_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    onDoneHandler onDone_;
    uint32_t readTimeout_ = 0;
    uint32_t writeTimeout_ = 0;
    std::string traceId_;
};
}//namespace small_http_client
