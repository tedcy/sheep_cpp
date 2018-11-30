#include "connector.h"
#include "event.h"
#include "socket.h"
#include "log.h"

Connector::Connector(EventLoop &loop,
        const std::string &addr, int port) :
    loop_(loop),
    addr_(addr), port_(port),
    socket_(new Socket){
}

void Connector::Connect(std::string &errMsg) {
    if (newConnectionHandler_ == nullptr) {
        errMsg = "invalid new connection handler";
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
    event_ = std::make_shared<Event>(loop_, socket_->GetFd());
    std::weak_ptr<Event> weakEvent = event_;
    event_->SetWriteEvent([weakEvent, this](){
        auto event = weakEvent.lock();
        if (!event) {
            //when server destroyed but loop running
            LOG(WARNING) << "Acceptor has been destoryed";
            return;
        }
        writeHandler();
    });
    event_->EnableWriteNotify();
}

void Connector::SetNewConnectionHandler(newConnectionHandlerT handler) {
    newConnectionHandler_ = handler;
}

void Connector::writeHandler() {
    std::string errMsg;
    auto fd = socket_->CheckConnect(errMsg);
    if (!errMsg.empty()) {
        //LOG(ERROR) << errMsg;
        return;
    }
    event_->DisableWriteNotify();
    //move socket_ and event_ to new connection
    newConnectionHandler_(socket_, event_);
    event_ = nullptr;
}
