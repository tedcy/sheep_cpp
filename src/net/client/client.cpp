#include "client.h"
#include "connector.h"
#include "event.h"
#include "socket.h"
#include <memory>
#include "log.h"
#include "asyncer.h"

namespace sheep{
namespace net{
Client::Client(EventLoop &loop,
            const std::string &addr, int port) :
    loop_(loop), 
    addr_(addr), port_(port),
    //FIXME: is there a way to no use recursive lock
    lock_(small_lock::MakeRecursiveLock()){
}

//any thread
void Client::AsyncConnect(std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    if (!disconnectedHandler_ || !connectedHandler_) {
        errMsg = "invalid connected or disconnected handler";
        return;
    }
    connectCalled_ = true;
    connector_ = std::make_shared<Connector>(loop_, addr_, port_);
    asyncer_ = std::make_shared<Asyncer>(loop_);
    asyncer_->AsyncDo([this](const std::string &argErrMsg){
        //loop thread
        small_lock::UniqueGuard guard(lock_);
        if(!argErrMsg.empty()) {
            disconnectedHandler_(argErrMsg);
            return;
        }
        std::string errMsg;
        //safe, connector is composition by client
        connector_->SetNewConnectionHandler(
        [this](std::unique_ptr<Socket> &socket, std::shared_ptr<Event> event){
            //loop thread
            newConnectionHandler(socket, event);
        });
        connector_->SetConnectFailedHandler(
        [this](const std::string &errMsg) {
            //loop thread
            disconnectedHandler_(errMsg);
        });
        connector_->Connect(errMsg);
        if (!errMsg.empty()) {
            disconnectedHandler_(errMsg);
            return;
        }
    });
}

//any thread
void Client::SetConnectedHandler(connectedHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    if (connectCalled_) {
        LOG(FATAL) << "AsyncConnected has been Called";
    }
    connectedHandler_ = handler;
}

//any thread
void Client::SetDisconnectedHandler(disconnectedHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    if (connectCalled_) {
        LOG(FATAL) << "AsyncConnected has been Called";
    }
    disconnectedHandler_ = handler;
}

//any thread
//FIXME: i'm not true if need lock
//because when called, connection_ must be set
TcpConnection& Client::GetTcpConnection() {
    return *connection_;
}

void Client::newConnectionHandler(std::unique_ptr<Socket> &socket, 
        std::shared_ptr<Event> event) {
    connection_ = std::make_shared<TcpConnection>(
            loop_, socket, event);
    std::string errMsg;
    //safe
    connection_->SetFinishHandler([this](
    std::string &errMsg, std::shared_ptr<TcpConnection> connection){
        //FIXME: why I need do this
        connection_ = nullptr;
        disconnectedHandler_(errMsg);
    });
    connection_->InitConnected(errMsg);
    //avoid handler change errMsg
    if (!errMsg.empty()) {
        disconnectedHandler_(errMsg);
        return;
    }
    connectedHandler_(errMsg);
}
}
}
