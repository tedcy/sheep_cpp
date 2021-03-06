#include "connector.h"
#include "event.h"
#include "socket.h"
#include "log.h"
#include "epoller.h"

namespace sheep{
namespace net{
Connector::Connector(EventLoop &loop,
        const std::string &addr, int port) :
    loop_(loop),
    addr_(addr), port_(port),
    socket_(new Socket){
}

Connector::~Connector() {
}

void Connector::Connect(std::string &errMsg) {
    if (!newConnectionHandler_ || !connectFailedHandler_) {
        errMsg = "invalid new connection or connect failed handler";
        return;
    }
    socket_->SetNoblock(errMsg);
    if (!errMsg.empty()) {
        return;
    }
    socket_->SetNoDelay(errMsg);
    if (!errMsg.empty()) {
        return;
    }
    socket_->Connect(errMsg, addr_, port_);
    if (!errMsg.empty()) {
        return;
    }
    event_ = std::make_shared<Event>(loop_, EpollerFactory::Get()->GetPollerType(), socket_->GetFd());
    std::weak_ptr<Event> weakEvent = event_;
    event_->SetWriteEvent([weakEvent, this](){
        auto event = weakEvent.lock();
        if (!event) {
            //when server destroyed but loop running
            LOG(WARNING) << "Connector has been destoryed";
            return;
        }
        event_->DisableWriteNotify();
        writeHandler();
    });
    event_->EnableWriteNotify();
}

void Connector::SetNewConnectionHandler(newConnectionHandlerT handler) {
    newConnectionHandler_ = handler;
}
    
void Connector::SetConnectFailedHandler(connectFailedHandlerT handler) {
    connectFailedHandler_ = handler;
}

void Connector::writeHandler() {
    std::string errMsg;
    socket_->CheckConnect(errMsg);
    if (!errMsg.empty()) {
        //LOG(ERROR) << errMsg;
        connectFailedHandler_(errMsg);
        return;
    }
    //move socket_ and event_ to new connection
    newConnectionHandler_(socket_, event_);
}
}
}
