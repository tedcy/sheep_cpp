#include "connection.h"
#include "log.h"

namespace small_http_client{
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
Connection::Connection(boost::asio::io_service &ios, 
        const std::string &host,const std::string &port):
    host_(host), port_(port), buffer_(),
    socket_(ios), resolver_(ios),
    connectedCallback_(), rwCallback_() {
}
void Connection::init(const argErrMsgCallback &connectedCallback){
    connectedCallback_ = connectedCallback;
    resolver_.async_resolve(host_, port_, 
            std::bind(
                &Connection::onResolve, 
                this,
                std::placeholders::_1,
                std::placeholders::_2));
}
void Connection::asyncWrite(http::request<http::string_body> &req, 
        const argErrMsgCallback &callback) {
    rwCallback_ = callback;
    http::async_write(socket_, req, std::bind(
                    &Connection::onWrite,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));
}
void Connection::asyncRead(http::response<http::string_body> &resp, 
        const argErrMsgCallback &callback) {
    rwCallback_ = callback;
    http::async_read(socket_, buffer_, resp, std::bind(
                    &Connection::onRead,
                    this,
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
                &Connection::onConnect, this,
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
        rwCallback_("write failed " + ec.message());
        return;
    }
    rwCallback_("");
}
void Connection::onRead(boost::system::error_code ec,
        std::size_t bytes_transferred){
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        rwCallback_("read failed " + ec.message());
        return;
    }
    rwCallback_("");
}
}//namespace small_http_client
