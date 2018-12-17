#include "tcp_connection.h"
#include "socket.h"
#include "event.h"
#include "log.h"
#include "epoller.h"

namespace sheep{
namespace net{
TcpConnection::TcpConnection(EventLoop &loop, int fd) :
    socket_(new Socket(fd)),
    event_(std::make_shared<Event>(loop, EpollerFactory::Get()->GetPollerType(), fd)){
}

TcpConnection::TcpConnection(EventLoop &loop,
        std::unique_ptr<Socket> &socket,
        std::shared_ptr<Event> event) :
    socket_(std::move(socket)), 
    event_(event){
}

TcpConnection::~TcpConnection() {
    //LOG(DEBUG) << "~TcpConnection";
}

void TcpConnection::InitAccepted(std::string &errMsg) {
    if (finishHandler_ == nullptr) {
        errMsg = "invalid finish handler";
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
    std::weak_ptr<TcpConnection> weakThis = shared_from_this();
    event_->SetReadEvent([weakThis, this](){
        auto realThis = weakThis.lock();
        if (!realThis) {
            LOG(WARNING) << "TcpConnection has been destoryed";
            return;
        }
        //always notify
        //event_->DisableReadNotify();
        readHandler();
    });
    event_->EnableReadNotify();
}

void TcpConnection::InitConnected(std::string &errMsg) {
    if (finishHandler_ == nullptr) {
        errMsg = "invalid finish handler";
        return;
    }
    std::weak_ptr<TcpConnection> weakThis = shared_from_this();
    event_->SetReadEvent([weakThis, this](){
        auto realThis = weakThis.lock();
        if (!realThis) {
            LOG(WARNING) << "TcpConnection has been destoryed";
            return;
        }
        //always notify
        //event_->DisableReadNotify();
        readHandler();
    });
    event_->EnableReadNotify();
}

void TcpConnection::AsyncRead(uint64_t expectSize,
        readHandlerT handler) {
    anyFlag_ = false;
    if (readedSize_ >= expectSize) {
        std::string errMsg;
        ResetRead();
        handler(errMsg);
        return;
    }
    expectSize_ = expectSize;
    userReadHandler_ = handler;
}

void TcpConnection::AsyncReadAny(readHandlerT handler) {
    anyFlag_ = true;
    if (readedSize_ != 0) {
        userReadHandler_ = handler;
        std::string errMsg;
        ResetRead();
        handler(errMsg);
        return;
    }
    userReadHandler_ = handler;
}

void TcpConnection::AsyncWrite(writeHandlerT handler) {
    userWriteHandler_ = handler;
    std::weak_ptr<TcpConnection> weakThis = shared_from_this();
    event_->SetWriteEvent([weakThis, this](){
        auto realThis = weakThis.lock();
        if (!realThis) {
            LOG(WARNING) << "TcpConnection has been destoryed";
            return;
        }
        //not notify until data's recv finished
        //event_->DisableWriteNotify();
        writeHandler();
    });
    event_->EnableWriteNotify();
}

void TcpConnection::readHandler() {
    std::string errMsg;
    //TODO buffer can be optimized to zero copy
    char buf[1024];
    auto count = socket_->Read(errMsg, buf, 1024);
    if (!errMsg.empty()) {
        if (userReadHandler_ != nullptr) {
            ResetRead();
            userReadHandler_(errMsg);
        }
        Finish(errMsg);
        return;
    }
    if (count < 0) {
        return;
    }
    if (count == 0) {
        Finish(errMsg);
        return;
    }
    ReadBuffer_.Write(buf, count);
    readedSize_ += count;
    if (anyFlag_ || readedSize_ >= expectSize_) {
        if (userReadHandler_ != nullptr) {
            ResetRead();
            userReadHandler_(errMsg);
        }
        return;
    }
}

void TcpConnection::writeHandler() {
    std::string errMsg;
    char buf[1024];
    int64_t count = WriteBuffer_.Read(buf, 1024);
    if (count == 0) {
        userWriteHandler_(errMsg);
        event_->DisableWriteNotify();
        return;
    }
    count = socket_->Write(errMsg, buf, count);
    WriteBuffer_.UpdateReadIndex(count);
    if (!errMsg.empty()) {
        userWriteHandler_(errMsg);
        Finish(errMsg);
        return;
    }
    if (count < 0) {
        return;
    }
    //can't accessed
}

void TcpConnection::Finish(std::string &errMsg) {
    finishHandler_(errMsg, shared_from_this());
}
}
}
