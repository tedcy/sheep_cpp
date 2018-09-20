#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/optional.hpp>

#include <string>
#include <functional>

#include "small_timer_factory.h"

namespace small_http_client{
//TODO timeout
class Connection: public std::enable_shared_from_this<Connection> {
    using argErrMsgCallback = 
        std::function<void(const std::string& errMsg)>;
    using argRespAndErrMsgCallback = 
        std::function<void(boost::beast::http::response<
                boost::beast::http::string_body> &resp,
        const std::string& errMsg)>;
public:
    ~Connection();
    void asyncWrite(boost::beast::http::request<boost::beast::http::string_body> &req, 
            const argErrMsgCallback &callback);
    void asyncRead(const argRespAndErrMsgCallback &callback);
    void GetLocalIp(std::string &ip);
private:
    friend class ConnectionPool;
    Connection(boost::asio::io_service &ios,
            const std::string &host,const std::string &port);
    void init(const argErrMsgCallback &connectedCallback);
    void StartRead();
    void SetClose(const argRespAndErrMsgCallback &callback);
    void Reset();
private:
    void onResolve(boost::system::error_code ec,
            boost::asio::ip::tcp::resolver::results_type results);
    void onConnect(boost::system::error_code ec);
    void onWrite(boost::system::error_code ec,
            std::size_t bytes_transferred);
    void onRead(boost::system::error_code ec,
            std::size_t bytes_transferred);
    void onClose(boost::system::error_code ec,
            std::size_t bytes_transferred);
    std::string host_;
    std::string port_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<small_timer::TimerI> timer_;
    //fix here in optiional
    //see https://github.com/boostorg/beast/issues/971
    boost::optional<
        boost::beast::http::response<
        boost::beast::http::string_body>> resp_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    argErrMsgCallback connectedCallback_;
    argRespAndErrMsgCallback closeCallback_;
    argRespAndErrMsgCallback rCallback_;
    argErrMsgCallback wCallback_;
};
}//namespace small_http_client
