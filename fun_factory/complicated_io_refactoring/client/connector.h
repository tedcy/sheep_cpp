#pragma once
#include "event_loop.h"
#include <memory>
#include <string>
#include <functional>

#include "small_packages.h"

class Event;
class Socket;

class Connector: public small_packages::noncopyable {
using newConnectionHandlerT = std::function<
    void(std::unique_ptr<Socket> &,std::shared_ptr<Event>)>;
public:
    Connector(EventLoop &loop,
            const std::string &addr, int port);
    void Connect(std::string &errMsg);
    void SetNewConnectionHandler(newConnectionHandlerT);
private:
    void writeHandler();

    //associated
    EventLoop &loop_;

    //composition
    std::string addr_;
    int port_;
    std::shared_ptr<Event> event_;
    std::unique_ptr<Socket> socket_;
    newConnectionHandlerT newConnectionHandler_;
};
