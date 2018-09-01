#pragma once
#include <boost/beast/http.hpp>

#include <json/json.h>

#include <string>
#include <map>
#include <functional>

namespace small_http_client {
typedef std::map<std::string,std::string> QueryStrings;
typedef std::map<std::string,std::string> Headers;

class Connection;
class ConnectionPool;

class Async {
public:
    Async(const std::string &method,const std::string &host,const std::string &port, 
            const std::string &target,const std::string &req, 
            const std::function<void(const std::string&, const std::string&)> &onDone);
    ~Async() = default;
    void setQueryStrings(std::shared_ptr<QueryStrings> queryStrings);
    void setHeaders(std::shared_ptr<Headers> headers);
    void doReq();
private:
    void onWrite(const std::string &errMsg);
    void onRead(const std::string &errMsg);
    void setMethod(const std::string &method);
    void setReq(const std::string &req);
    std::shared_ptr<Connection> connection_ = nullptr;
    std::shared_ptr<ConnectionPool> connectionPool_ = nullptr;
    const std::string host_;
    const std::string port_;
    const std::string target_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    boost::beast::http::response<boost::beast::http::string_body> resp_;
    const std::function<void(const std::string&, const std::string&)> onDone_;
};
}//namespace small_http_client
