#pragma once
#include "client/client.h"
#include "net/common/tcp/co_tcp_connection.h"
#include "coroutine/coroutine_scheduler.h"

namespace sheep{
namespace net{

class CoClient {
public:
    CoClient(const std::string &addr, int port);
    CoTcpConnection Connect(std::string &errMsg);

private:
    Client client_;
};

}
}