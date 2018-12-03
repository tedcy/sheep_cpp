#pragma once
#include "event_loop.h"
#include "tcp_connection.h"
#include <functional>
#include "small_packages.h"
#include <set>

namespace sheep{
namespace net{
class Acceptor;

class Server: public small_packages::noncopyable{
using connectedHandlerT = std::function<void(std::string &errMsg,
        TcpConnection&)>;
using disconnectedHandlerT = std::function<void(std::string &errMsg)>;
public:
    Server(EventLoop &loop, 
            const std::string &addr, int fd);
    void Init(std::string &errMsg);
    void SetConnectedHandler(connectedHandlerT);
    void SetDisconnectedHandler(disconnectedHandlerT);
private:
    void newConnectionHandler(int fd);
    EventLoop &loop_;

    //composition
    std::shared_ptr<Acceptor> acceptor_;
    connectedHandlerT connectedHandler_;
    disconnectedHandlerT disconnectedHandler_;

    using TcpConnectionSet = std::set<std::shared_ptr<TcpConnection>>;
    std::shared_ptr<TcpConnectionSet> connections_;
};
}
}
