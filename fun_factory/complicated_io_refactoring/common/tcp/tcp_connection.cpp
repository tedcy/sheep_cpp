#include "tcp_connection.h"
#include "socket.h"
#include "event.h"

TcpConnection::TcpConnection(EventLoop &loop, int fd) :
    socket_(new Socket(fd)),
    event_(std::make_shared<Event>(loop, fd)){
}

void TcpConnection::Init(std::string &errMsg) {
    if (messageHandler_ == nullptr) {
        errMsg = "invalid message handler";
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
    //FIXME: not safe
    event_->SetReadEvent([this](){
                readHandler();
            });
    event_->EnableReadNotify();
}

void TcpConnection::SetMessageHandler(messageHandlerT handler) {
    messageHandler_ = handler;
}

void TcpConnection::readHandler() {
    std::string errMsg;
    //TODO buffer can be optimized to zero copy
    char buf[1024];
    auto count = socket_->Read(errMsg, buf, 1024);
    ReadBuffer_.Write(buf, count);
    messageHandler_(errMsg, *this);
}
