#include "server.h"
#include "acceptor.h"
#include "event.h"
#include "socket.h"
#include <memory>
#include "log.h"
#include "asyncer.h"

namespace sheep{
namespace net{
Server::Server(EventLoop &loop,
       const std::string &addr, int fd) :
    loop_(loop),
    lock_(small_lock::MakeLock()),
    acceptor_(std::make_shared<Acceptor>(loop_, addr, fd)),
    connections_(std::make_shared<TcpConnectionSet>()){
}

//any thread
void Server::Serve(std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    if (serveCalled_) {
        errMsg = "Serve can't called twice";
        return;
    }
    if (!connectedHandler_ || !disconnectedHandler_) {
        errMsg = "invalid connected or disconnected handler";
        return;
    }
    serveCalled_ = true;
    asyncer_ = std::make_shared<Asyncer>(loop_);
    //safe, asyncer is composition by server
    asyncer_->AsyncDo([this](const std::string &argErrMsg){
        small_lock::UniqueGuard guard(lock_);
        if (!argErrMsg.empty()) {
            disconnectedHandler(argErrMsg);
            return;
        }
        std::string errMsg;
        //safe, acceptor is composition by server
        acceptor_->SetNewConnectionHandler([this](int fd){
            newConnectionHandler(fd);
        });
        acceptor_->Listen(errMsg);
        if (!errMsg.empty()) {
            disconnectedHandler(errMsg);
            return;
        }
    });
}

//any thread
void Server::SetConnectedHandler(connectedHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    if (serveCalled_) {
        LOG(FATAL) << "Serve has been Called";
    }
    connectedHandler_ = handler;
}

//any thread
void Server::SetDisconnectedHandler(disconnectedHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    if (serveCalled_) {
        LOG(FATAL) << "Serve has been Called";
    }
    disconnectedHandler_ = handler;
}
    
void Server::connectedHandler(const std::string &errMsg, TcpConnection &connection) {
    connectedHandler_(errMsg, connection);
}

void Server::disconnectedHandler(const std::string &errMsg) {
    disconnectedHandler_(errMsg);
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
            LOG(WARNING) << "server has been destoryed";
            return;
        }
        connections_->erase(connection);
        disconnectedHandler(errMsg);
    });
    connection->InitAccepted(errMsg);
    //avoid handler change errMsg
    if (errMsg != "") {
        connectedHandler_(errMsg, *connection);
        return;
    }
    connectedHandler(errMsg, *connection);
    connections_->insert(connection);
}
}
}
