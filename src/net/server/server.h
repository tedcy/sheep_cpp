#pragma once
#include "net/common/event_loop.h"
#include "net/common/tcp/tcp_connection.h"
#include <functional>
#include "extends/small_packages/small_packages.h"
#include <set>

namespace sheep{
namespace net{
class Acceptor;
class Asyncer;

class Server: public small_packages::noncopyable{
using connectedHandlerT = std::function<void(const std::string &errMsg,
        TcpConnection&)>;
using disconnectedHandlerT = std::function<void(const std::string &errMsg)>;
public:
    Server(EventLoop &loop, 
            const std::string &addr, int fd);
    void Serve(std::string &errMsg);
    void SetConnectedHandler(connectedHandlerT);
    void SetDisconnectedHandler(disconnectedHandlerT);
private:
    void connectedHandler(const std::string &errMsg, TcpConnection &connection);
    void disconnectedHandler(const std::string &errMsg);
    void newConnectionHandler(int fd);
    EventLoop &loop_;

    //composition
    //connectedHandler_,disconnectedHandler_,serveCalled_,asyncer_ add lock
    std::shared_ptr<small_lock::LockI> lock_;
    bool serveCalled_ = false;
    std::shared_ptr<Acceptor> acceptor_;
    std::shared_ptr<Asyncer> asyncer_;
    connectedHandlerT connectedHandler_;
    disconnectedHandlerT disconnectedHandler_;

    using TcpConnectionSet = std::set<std::shared_ptr<TcpConnection>>;
    std::shared_ptr<TcpConnectionSet> connections_;
};
}
}
