#pragma once

#include "net/server/server.h"
#include "coroutine/coroutine_scheduler.h"
#include "net/common/tcp/co_tcp_connection.h"

namespace sheep{
namespace net{

class CoServer {
using connectedHandlerT = std::function<void(const std::string &errMsg,
        CoTcpConnection&)>;
public:
    CoServer(CoroutineScheduler &scheduler, 
            const std::string &addr, int fd, const connectedHandlerT&);
    void Serve(std::string &errMsg);
private:
    Server server_;
    connectedHandlerT connectedHandler_;
};

}
}