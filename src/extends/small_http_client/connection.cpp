#include "connection.h"
#include "log.h"

namespace small_http_client{
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
Connection::Connection(boost::asio::io_service &ios, 
        const std::string &host,const std::string &port):
    host_(host), port_(port), buffer_(),
    socket_(ios), resolver_(ios),
    connectedCallback_(), rCallback_(), wCallback_() {
}
void Connection::init(const argErrMsgCallback &connectedCallback){
    connectedCallback_ = connectedCallback;
    resolver_.async_resolve(host_, port_, 
            std::bind(
                &Connection::onResolve, 
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
}
void Connection::GetLocalIp(std::string &ip) {
    ip = socket_.local_endpoint().address().to_v4().to_string();
}
void Connection::asyncWrite(http::request<http::string_body> &req, 
        const argErrMsgCallback &callback) {
    wCallback_ = callback;
    http::async_write(socket_, req, std::bind(
                    &Connection::onWrite,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2));
}
void Connection::asyncRead(http::response<http::string_body> &resp, 
        const argErrMsgCallback &callback) {
    rCallback_ = callback;
    http::async_read(socket_, buffer_, resp, std::bind(
                    &Connection::onRead,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2));
}
void Connection::onResolve(boost::system::error_code ec,
        tcp::resolver::results_type results) {
    if (ec) {
        connectedCallback_("resolve failed " + ec.message());
        return;
    }
    boost::asio::async_connect(
            socket_, results.begin(), results.end(),
            std::bind(
                &Connection::onConnect,
                shared_from_this(),
                std::placeholders::_1));
}
void Connection::onConnect(boost::system::error_code ec) {
    if (ec) {
        connectedCallback_("connect failed " + ec.message());
        return;
    }
    connectedCallback_("");
}
void Connection::onWrite(boost::system::error_code ec,
        std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        wCallback_("write failed " + ec.message());
        wCallback_ = nullptr;
        return;
    }
    wCallback_("");
    wCallback_ = nullptr;
}
void Connection::onRead(boost::system::error_code ec,
        std::size_t bytes_transferred){
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        rCallback_("read failed " + ec.message());
        rCallback_ = nullptr;
        return;
    }
    rCallback_("");
    rCallback_ = nullptr;
}
}//namespace small_http_client
