#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <functional>

namespace small_http_client{
//TODO timeout
class Connection: public std::enable_shared_from_this<Connection> {
    using argErrMsgCallback = std::function<void(const std::string& errMsg)>;
public:
    ~Connection() = default;
    void asyncWrite(boost::beast::http::request<boost::beast::http::string_body> &req, 
            const argErrMsgCallback &callback);
    void asyncRead(boost::beast::http::response<boost::beast::http::string_body> &resp, 
            const argErrMsgCallback &callback);
    void GetLocalIp(std::string &ip);
private:
    friend class ConnectionPool;
    Connection(boost::asio::io_service &ios,
            const std::string &host,const std::string &port);
    void init(const argErrMsgCallback &connectedCallback);
private:
    void onResolve(boost::system::error_code ec,
            boost::asio::ip::tcp::resolver::results_type results);
    void onConnect(boost::system::error_code ec);
    void onWrite(boost::system::error_code ec,
            std::size_t bytes_transferred);
    void onRead(boost::system::error_code ec,
            std::size_t bytes_transferred);
    std::string host_;
    std::string port_;
    boost::beast::flat_buffer buffer_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    argErrMsgCallback connectedCallback_;
    argErrMsgCallback rCallback_;
    argErrMsgCallback wCallback_;
};
}//namespace small_http_client
