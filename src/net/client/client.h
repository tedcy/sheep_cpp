#pragma once

#include "event_loop.h"
#include "tcp_connection.h"
#include <functional>

#include "small_packages.h"

namespace sheep{
namespace net{
class Connector;
class Asyncer;

class Client: public small_packages::noncopyable{
using connectedHandlerT = std::function<void(const std::string &errMsg,
        TcpConnection&)>;
using disconnectedHandlerT = std::function<void(const std::string &errMsg)>;
public:
    Client(EventLoop &loop,
            const std::string &addr, int port);
    void AsyncConnect(std::string &errMsg);
    void SetConnectedHandler(connectedHandlerT);
    void SetDisconnectedHandler(disconnectedHandlerT);
private:
    void newConnectionHandler(std::unique_ptr<Socket> &, std::shared_ptr<Event>);
    EventLoop &loop_;

    //composition
    std::shared_ptr<small_lock::LockI> lock_;
    bool connected_ = false;
    bool connectCalled_ = false;
    std::shared_ptr<Connector> connector_;
    std::shared_ptr<Asyncer> connectedHandlerAsyncer_;
    std::shared_ptr<Asyncer> connectAsyncer_;
    connectedHandlerT connectedHandler_;
    disconnectedHandlerT disconnectedHandler_;

    std::shared_ptr<TcpConnection> connection_;
};
}
}
