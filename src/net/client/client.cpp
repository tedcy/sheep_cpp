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
    lock_(small_lock::MakeLock()),
    connector_(std::make_shared<Connector>(loop, addr, port)){
}

//any thread
void Client::AsyncConnect(std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    connectCalled_ = true;
    if (!disconnectedHandler_ || !connectedHandler_) {
        errMsg = "invalid connected or disconnected handler";
        return;
    }
    asyncer_ = std::make_shared<Asyncer>(loop_);
    asyncer_->AsyncDo([this](const std::string &argErrMsg){
        //loop thread
        small_lock::UniqueGuard guard(lock_);
        if(!argErrMsg.empty()) {
            disconnectedHandler_(argErrMsg);
            asyncer_ = nullptr;
            return;
        }
        std::string errMsg;
        //safe, connector is composition by client
        connector_->SetNewConnectionHandler(
        [this](std::unique_ptr<Socket> &socket, std::shared_ptr<Event> event){
            //loop thread
            newConnectionHandler(socket, event);
        });
        connector_->Connect(errMsg);
        if (!errMsg.empty()) {
            disconnectedHandler_(errMsg);
            asyncer_ = nullptr;
            return;
        }
        asyncer_ = nullptr;
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
