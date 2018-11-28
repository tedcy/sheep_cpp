#pragma once

#include <memory>
#include <functional>
#include "small_packages.h"
#include "buffer.h"

class Event;
class EventLoop;
class Socket;

class TcpConnection: public small_packages::noncopyable{
using messageHandlerT = std::function<void(std::string &errMsg,
        TcpConnection&)>;
public:
    TcpConnection(EventLoop &loop, int fd);
    void Init(std::string &errMsg);
    void SetMessageHandler(messageHandlerT handler);
    Buffer ReadBuffer_;
    Buffer WriteBuffer_;
private:
    void readHandler();
    std::unique_ptr<Socket> socket_;
    std::shared_ptr<Event> event_;
    messageHandlerT messageHandler_;
};
