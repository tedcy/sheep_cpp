#pragma once

#include "event_loop.h"
#include "tcp_connection.h"
#include <functional>

#include "small_packages.h"

class Connector;

class Client: public small_packages::noncopyable{
using connectedHandlerT = std::function<void(std::string &errMsg,
        TcpConnection&)>;
using disconnectedHandlerT = std::function<void(std::string &errMsg)>;
public:
    Client(EventLoop &loop,
            const std::string &addr, int port);
    void Init(std::string &errMsg);
    void SetConnectedHandler(connectedHandlerT);
    void SetDisconnectedHandler(disconnectedHandlerT);
private:
    void newConnectionHandler(std::unique_ptr<Socket> &, std::shared_ptr<Event>);
    EventLoop &loop_;

    //composition
    std::shared_ptr<Connector> connector_;
    connectedHandlerT connectedHandler_;
    disconnectedHandlerT disconnectedHandler_;

    std::shared_ptr<TcpConnection> connection_;
};
