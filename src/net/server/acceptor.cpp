#include "acceptor.h"
#include "event.h"
#include "socket.h"
#include "log.h"
#include "epoller.h"

namespace sheep{
namespace net{
Acceptor::Acceptor(EventLoop &loop,
        const std::string &addr, int port) :
    loop_(loop),
    addr_(addr), port_(port), 
    socket_(new Socket){
}

Acceptor::~Acceptor() {
}

void Acceptor::Listen(std::string &errMsg) {
    if (newConnectionHandler_ == nullptr) {
        errMsg = "invalid new connection handler";
        return;
    }
    socket_->Bind(errMsg, addr_, port_);
    if (!errMsg.empty()) {
        return;
    }
    socket_->SetReuseAddr(errMsg);
    if (!errMsg.empty()) {
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
    event_ = std::make_shared<Event>(loop_, EpollerFactory::Get()->GetPollerType(), socket_->GetFd());
    std::weak_ptr<Event> weakEvent = event_;
    event_->SetReadEvent([weakEvent, this](){
        auto event = weakEvent.lock();
        if (!event) {
            //when server destroyed but loop running
            LOG(WARNING) << "Acceptor has been destoryed";
            return;
        }
        event_->DisableReadNotify();
        readHandler();
    });
    event_->EnableReadNotify();
    socket_->Listen(errMsg);
}

void Acceptor::SetNewConnectionHandler(std::function<void(int fd)> handler) {
    newConnectionHandler_ = handler;
}

void Acceptor::readHandler() {
    std::string errMsg;
    auto fd = socket_->Accept(errMsg);
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg;
        return;
    }
    newConnectionHandler_(fd);
}
}
}
