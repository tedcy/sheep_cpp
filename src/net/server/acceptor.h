#pragma once
#include "event_loop.h"
#include <memory>
#include <string>
#include <functional>

#include "small_packages.h"

class Event;
class Socket;

class Acceptor: public small_packages::noncopyable {
public:
    Acceptor(EventLoop &loop,
            const std::string &addr, int port);
    ~Acceptor();
    void Listen(std::string &errMsg);
    void SetNewConnectionHandler(
            std::function<void(int fd)>);
private:
    void readHandler();

    //associated
    EventLoop &loop_;

    //composition
    std::string addr_;
    int port_;
    std::shared_ptr<Event> event_;
    std::unique_ptr<Socket> socket_;
    std::function<void(int fd)> newConnectionHandler_;
};
