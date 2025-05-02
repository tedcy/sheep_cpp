#include "co_server.h"
#include "co_tcp_connection.h"
#include "coroutine_scheduler.h"
#include "log.h"

namespace sheep {
namespace net {

CoServer::CoServer(CoroutineScheduler &scheduler, const std::string &addr,
                   int fd, const connectedHandlerT &handler)
    : server_(scheduler.getLoop(), addr, fd), connectedHandler_(handler) {
    server_.SetConnectedHandler(
        [this](const std::string &errMsg, TcpConnection &conn) {
            CoroutineScheduler::currentCoroScheduler()->addCoroutine(
                [this, errMsg, &conn]() {
                    CoTcpConnection coConn(conn);
                    connectedHandler_(errMsg, coConn);
                });
        });
    server_.SetDisconnectedHandler([](const std::string &errMsg) {
        LOG(INFO) << "disconnected";
        if(!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
    });
}

void CoServer::Serve(std::string &errMsg) {
    server_.Serve(errMsg);
}
}

}