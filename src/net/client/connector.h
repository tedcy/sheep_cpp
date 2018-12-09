#pragma once
#include "event_loop.h"
#include <memory>
#include <string>
#include <functional>

#include "small_packages.h"

namespace sheep{
namespace net{
class Event;
class Socket;

class Connector: public small_packages::noncopyable {
using newConnectionHandlerT = std::function<
    void(std::unique_ptr<Socket> &,std::shared_ptr<Event>)>;
using connectFailedHandlerT = std::function<
    void(const std::string &errMsg)>;
public:
    Connector(EventLoop &loop,
            const std::string &addr, int port);
    ~Connector();
    void Connect(std::string &errMsg);
    void SetNewConnectionHandler(newConnectionHandlerT);
    void SetConnectFailedHandler(connectFailedHandlerT);
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
    connectFailedHandlerT connectFailedHandler_;
};
}
}
