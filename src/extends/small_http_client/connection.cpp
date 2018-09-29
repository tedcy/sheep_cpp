#include "connection.h"
#include "log.h"

using namespace small_log;

namespace small_http_client{
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
Connection::Connection(boost::asio::io_service &ios, 
        const std::string &host,const std::string &port):
    host_(host), port_(port), buffer_(), resp_(boost::beast::http::response<
            boost::beast::http::string_body>()),
    socket_(ios), resolver_(ios),
    connectedCallback_(), rCallback_(), wCallback_() {
}
Connection::~Connection() {
    //LOG(INFO) << "~Connection";
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
void Connection::StartRead() {
    http::async_read(socket_, buffer_, resp_.get(), std::bind(
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

void Connection::AsyncDo(uint32_t writeTimeout, uint32_t readTimeout,
            boost::beast::http::request<boost::beast::http::string_body> &req, 
            const argErrMsgCallback &writeCallback,
            const argRespAndErrMsgCallback &readCallback) {
    if (writeTimeout != 0) {
        writeTimer_ = small_timer::MakeTimer();
        auto self = shared_from_this();
        writeTimer_->AsyncWait(writeTimeout, 
            [this, self](const std::string &errMsg){
                if (errMsg == "") {
                    //LOG(INFO) << "time out";
                    socket_.cancel();
                }
            }
        );
    }else {
        writeTimer_ = nullptr;
    }
    if (readTimeout != 0) {
        readTimer_ = small_timer::MakeTimer();
        auto self = shared_from_this();
        readTimer_->AsyncWait(readTimeout, 
            [this, self](const std::string &errMsg){
                if (errMsg == "") {
                    //LOG(INFO) << "time out";
                    socket_.cancel();
                }
            }
        );
    }else {
        readTimer_ = nullptr;
    }
    wCallback_ = writeCallback;
    rCallback_ = readCallback;
    http::async_write(socket_, req, std::bind(
                    &Connection::onWrite,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2));
}

void Connection::onWrite(boost::system::error_code ec,
        std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if(writeTimer_) {
        writeTimer_->Cancel();
    }
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
    if(readTimer_) {
        readTimer_->Cancel();
    }
    if(ec) {
        available_ = false;
        boost::system::error_code ec1;
        if(rCallback_ != nullptr) {
            rCallback_("", "read failed " + ec.message());
            rCallback_ = nullptr;
        }
        closeCallback_("", "read failed " + ec.message());
        socket_.shutdown(tcp::socket::shutdown_both, ec1);
        return;
    }
    rCallback_(resp_.get().body(), "");
    rCallback_ = nullptr;
    Reset();
    StartRead();
}
void Connection::SetClose(const argRespAndErrMsgCallback& callback) {
    closeCallback_ = callback;
}
void Connection::Reset() {
    resp_.emplace(boost::beast::http::response<
            boost::beast::http::string_body>());
}
bool Connection::Available() {
    return available_;
}
}//namespace small_http_client
