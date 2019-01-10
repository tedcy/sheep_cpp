#include "tcp_connection.h"
#include "socket.h"
#include "event.h"
#include "log.h"
#include "epoller.h"
#include "asyncer.h"
#include "small_packages.h"

namespace sheep{
namespace net{
TcpConnection::TcpConnection(EventLoop &loop, int fd) :
    socket_(new Socket(fd)),
    event_(std::make_shared<Event>(loop, EpollerFactory::Get()->GetPollerType(), fd)),
    lock_(small_lock::MakeRecursiveLock()){
}

TcpConnection::TcpConnection(std::unique_ptr<Socket> &socket,
        std::shared_ptr<Event> &event) :
    socket_(std::move(socket)), 
    event_(std::move(event)),
    lock_(small_lock::MakeRecursiveLock()){
}

TcpConnection::~TcpConnection() {
    //LOG(DEBUG) << "~TcpConnection";
}

void TcpConnection::Reset() {
    finished_ = false;
    userReadHandler_ = nullptr;
    writeDone_ = false;
    userWriteHandler_ = nullptr;
    anyFlag_ = false;
    ReadBuffer_.Reset();
    WriteBuffer_.Reset();
    ResetRead();
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
    small_lock::UniqueGuard guard(lock_);
    if (finished_) {
        handler("Finished");
        return;
    }
    std::weak_ptr<TcpConnection> weakThis = shared_from_this();
    asyncer_ = std::make_shared<Asyncer>(event_->GetLooper());
    asyncer_->AsyncDo([this, weakThis, handler](const std::string &errMsg){
        if (!errMsg.empty()) {
            handler(errMsg);
            return;
        }
        auto realThis = weakThis.lock();
        if (!realThis) {
            handler("TcpConnection has been destoryed");
            return;
        }
        small_lock::UniqueGuard guard(lock_);
        userWriteHandler_ = handler;
        std::weak_ptr<TcpConnection> weakThis = shared_from_this();
        event_->SetWriteEvent([weakThis, this](){
            auto realThis = weakThis.lock();
            if (!realThis) {
                userWriteHandler_("TcpConnection has been destoryed");
                return;
            }
            //not notify until data's recv finished
            //event_->DisableWriteNotify();
            writeHandler();
        });
        event_->EnableWriteNotify();
    });
}

void TcpConnection::readHandler() {
    small_lock::UniqueGuard guard(lock_);
    std::string errMsg;
    //TODO buffer can be optimized to zero copy
    char buf[1024];
    auto count = socket_->Read(errMsg, buf, 1024);
    if (count == 0) {
        errMsg = "Finished";
    }
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
    ReadBuffer_.Write(buf, count);
    readedSize_ += count;
    if (anyFlag_ || readedSize_ >= expectSize_) {
        if (userReadHandler_ != nullptr) {
            ResetRead();
            userReadHandler_("");
        }
        return;
    }
}

void TcpConnection::writeHandler() {
    small_lock::UniqueGuard guard(lock_);
    std::string errMsg;
    char buf[1024];
    int64_t count = WriteBuffer_.Read(buf, 1024);
    if (count == 0) {
        writeDone_ = true;
        event_->DisableWriteNotify();
        userWriteHandler_(errMsg);
        return;
    }
    count = socket_->Write(errMsg, buf, count);
    WriteBuffer_.UpdateReadIndex(count);
    if (!errMsg.empty()) {
        writeDone_ = true;
        userWriteHandler_(errMsg);
        Finish(errMsg);
        return;
    }
    if (count < 0) {
        return;
    }
    //can't accessed
}

void TcpConnection::Finish(const std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    if (finished_) {
        return;
    }
    finished_ = true;
    if (asyncer_) {
        asyncer_->Cancel();
    }
    if (!writeDone_ && userWriteHandler_) {
        writeDone_ = true;
        userWriteHandler_("Finished");
    }
    event_ = nullptr;
    auto sharedPtr = shared_from_this();
    finishHandler_(errMsg, sharedPtr);
}

void TcpConnection::GetLocalIp(std::string &errMsg, std::string &ip) {
    if (socket_ == nullptr) {
        errMsg = "socket is nullptr";
        return;
    }
    socket_->GetLocalIp(errMsg, ip);
}
void TcpConnection::WriteBufferPush(const char* buf, uint64_t len) {
    small_lock::UniqueGuard guard(lock_);
    WriteBuffer_.Push(buf, len);
}
uint64_t TcpConnection::ReadBufferPopHead(char *buf, uint64_t len) {
    small_lock::UniqueGuard guard(lock_);
    return ReadBuffer_.PopHead(buf, len);
}
}
}
