#pragma once

#include "net/common/event_loop.h"
#include "net/common/tcp/tcp_connection.h"
#include <functional>

#include "extends/small_packages/small_packages.h"

namespace sheep{
namespace net{
class Connector;
class Asyncer;
class ClientPool;

//can be reuse to reconnect
class Client: public small_packages::noncopyable{
using connectedHandlerT = std::function<void(const std::string &errMsg)>;
using disconnectedHandlerT = std::function<void(const std::string &errMsg)>;
public:
    Client(EventLoop &loop,
            const std::string &addr, int port);
    ~Client();
    void AsyncConnect(std::string &errMsg);
    void SetConnectedHandler(connectedHandlerT);
    void SetDisconnectedHandler(disconnectedHandlerT);
    TcpConnection& GetTcpConnection();
    const std::string& GetAddr() {
        return addr_;
    }
    int GetPort() {
        return port_;
    }
private:
    friend ClientPool;
    void Reset(const std::string &errMsg);
    void newConnectionHandler(std::unique_ptr<Socket> &, std::shared_ptr<Event>&);
    EventLoop &loop_;
    const std::string addr_;
    const int port_ = 0;

    //composition
    //connectedHandler_,disconnectedHandler_,connectCalled_,connectAsyncer_ add lock
    std::shared_ptr<small_lock::LockI> lock_;
    bool connectCalled_ = false;
    std::shared_ptr<Connector> connector_;
    std::shared_ptr<Asyncer> asyncer_;
    connectedHandlerT connectedHandler_;
    disconnectedHandlerT disconnectedHandler_;

    std::shared_ptr<TcpConnection> connection_;
};
}
}
