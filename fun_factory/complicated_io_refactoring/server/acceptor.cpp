#include "acceptor.h"
#include "event.h"
#include "socket.h"
#include "log.h"

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
    event_ = std::make_shared<Event>(loop_, socket_->GetFd());
    //FIXME: not safe
    event_->SetReadEvent([this](){
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
