#include "server.h"
#include "acceptor.h"
#include "event.h"
#include "socket.h"
#include <memory>
#include "log.h"

namespace sheep{
namespace net{
Server::Server(EventLoop &loop,
       const std::string &addr, int fd) :
    loop_(loop),
    acceptor_(std::make_shared<Acceptor>(loop_, addr, fd)),
    connections_(std::make_shared<TcpConnectionSet>()){
}

void Server::Serve(std::string &errMsg) {
    if (connectedHandler_ == nullptr) {
        errMsg = "invliad connectedHandler";
        return;
    }
    //safe, acceptor is composition by server
    acceptor_->SetNewConnectionHandler([this](int fd){
                newConnectionHandler(fd);
            });
    acceptor_->Listen(errMsg);
}

void Server::SetConnectedHandler(connectedHandlerT handler) {
    connectedHandler_ = handler;
}

void Server::SetDisconnectedHandler(disconnectedHandlerT handler) {
    disconnectedHandler_ = handler;
}

void Server::newConnectionHandler(int fd) {
    auto connection = std::make_shared<TcpConnection>(loop_, fd);
    std::string errMsg;
    std::weak_ptr<TcpConnectionSet> weakConnectionSet
        = connections_;
    connection->SetFinishHandler([weakConnectionSet, this](
    std::string &errMsg, std::shared_ptr<TcpConnection> connection){
        auto realConnectionSet = weakConnectionSet.lock();
        if (!realConnectionSet) {
            //LOG(WARNING) << "server has been destoryed";
            return;
        }
        connections_->erase(connection);
        if (disconnectedHandler_ != nullptr) {
            disconnectedHandler_(errMsg);
        }
    });
    connection->InitAccepted(errMsg);
    //avoid handler change errMsg
    if (errMsg != "") {
        connectedHandler_(errMsg, *connection);
        return;
    }
    connectedHandler_(errMsg, *connection);
    connections_->insert(connection);
}
}
}
