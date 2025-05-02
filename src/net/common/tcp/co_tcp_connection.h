#pragma once

#include "net/common/tcp/tcp_connection.h"

namespace sheep{
namespace net{

//异步模式下，在AsyncRead or
// AsyncWrite下使用TcpConnection不管怎么样都不会出现问题，因为Async的回调执行完以后才会析构TcpConnection
//协程模式下，AsyncRead or
// AsyncWrite下使用TcpConnection会出现问题，因为Async的回调早就执行完毕了，TcpConnection已经析构了，
// 所以CoTcpConnection需要一个weakPtr的TcpConnection
// 来延长TcpConnection的生命周期
class CoTcpConnection {
public:
    CoTcpConnection() = default;
    CoTcpConnection(TcpConnection &connection);
    CoTcpConnection(std::shared_ptr<TcpConnection> connection);
    int Write(std::string& errMsg, const char *buf, int len);
    int Read(std::string& errMsg, char *buf, int len);
    void Finish();
private:
    TcpConnection* connection_ = nullptr;
    std::shared_ptr<TcpConnection> connection_sptr_;
};

}
}
