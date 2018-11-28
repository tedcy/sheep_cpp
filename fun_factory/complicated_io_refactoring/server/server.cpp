#include "server.h"
#include "acceptor.h"
#include "event.h"
#include "socket.h"
#include <memory>

Server::Server(EventLoop &loop,
       const std::string &addr, int fd) :
    loop_(loop),
    acceptor_(std::make_shared<Acceptor>(loop_, addr, fd)){
}

void Server::Init(std::string &errMsg) {
    //FIXME: not safe
    acceptor_->SetNewConnectionHandler([this](int fd){
                newConnectionHandler(fd);
            });
    acceptor_->Listen(errMsg);
}

void Server::SetMessageHandler(messageHandlerT messageHandler) {
    messageHandler_ = messageHandler;
}

void Server::SetConnectedHandler(connectedHandlerT connectedHandler) {
    connectedHandler_ = connectedHandler;
}

void Server::newConnectionHandler(int fd) {
    auto connection = std::make_shared<TcpConnection>(loop_, fd);
    std::string errMsg;
    connection->SetMessageHandler(messageHandler_);
    connection->Init(errMsg);
    if (connectedHandler_ != nullptr) {
        connectedHandler_(errMsg);
    }
    connections_.insert(connection);
}
