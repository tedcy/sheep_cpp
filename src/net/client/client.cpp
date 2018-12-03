#include "client.h"
#include "connector.h"
#include "event.h"
#include "socket.h"
#include <memory>
#include "log.h"

namespace sheep{
namespace net{
Client::Client(EventLoop &loop,
            const std::string &addr, int port) :
    loop_(loop), 
    connector_(std::make_shared<Connector>(loop, addr, port)){
}
void Client::Init(std::string &errMsg) {
    if (connectedHandler_ == nullptr) {
        errMsg = "invliad connectedHandler";
        return;
    }
    //safe, connector is composition by server
    connector_->SetNewConnectionHandler(
    [this](std::unique_ptr<Socket> &socket, std::shared_ptr<Event> event){
        newConnectionHandler(socket, event);
    });
    connector_->Connect(errMsg);
}
void Client::SetConnectedHandler(connectedHandlerT handler) {
    connectedHandler_ = handler;
}
void Client::SetDisconnectedHandler(disconnectedHandlerT handler) {
    disconnectedHandler_ = handler;
}
void Client::newConnectionHandler(std::unique_ptr<Socket> &socket, 
        std::shared_ptr<Event> event) {
    connection_ = std::make_shared<TcpConnection>(
            loop_, socket, event);
    std::string errMsg;
    std::weak_ptr<TcpConnection> weakConnection
        = connection_;
    connection_->SetFinishHandler([weakConnection, this](
    std::string &errMsg, std::shared_ptr<TcpConnection> connection){
        auto realConnection = weakConnection.lock();
        if (!realConnection) {
            //LOG(WARNING) << "server has been destoryed";
            return;
        }
        connection_ = nullptr;
        if (disconnectedHandler_ != nullptr) {
            disconnectedHandler_(errMsg);
        }
    });
    connection_->InitConnected(errMsg);
    //avoid handler change errMsg
    if (errMsg != "") {
        connectedHandler_(errMsg, *connection_);
        return;
    }
    connectedHandler_(errMsg, *connection_);
}
}
}
