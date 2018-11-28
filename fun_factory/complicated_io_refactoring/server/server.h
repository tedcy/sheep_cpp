#pragma once
#include "event_loop.h"
#include "tcp_connection.h"
#include <functional>
#include "small_packages.h"
#include <set>

class Acceptor;

class Server: small_packages::noncopyable{
using messageHandlerT = std::function<void(std::string &errMsg,
        TcpConnection&)>;
using connectedHandlerT = std::function<void(std::string &errMsg)>;
public:
    Server(EventLoop &loop, 
            const std::string &addr, int fd);
    void Init(std::string &errMsg);
    void SetMessageHandler(messageHandlerT);
    void SetConnectedHandler(connectedHandlerT);
private:
    void newConnectionHandler(int fd);
    EventLoop &loop_;

    //composition
    std::shared_ptr<Acceptor> acceptor_;
    messageHandlerT messageHandler_;
    connectedHandlerT connectedHandler_;

    std::set<std::shared_ptr<TcpConnection>> connections_;
};
