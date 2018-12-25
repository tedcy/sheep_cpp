#pragma once

#include <memory>
#include <functional>
#include "small_packages.h"
#include "buffer.h"
#include "small_packages.h"

namespace sheep{
namespace net{
class Event;
class EventLoop;
class Socket;
class Asyncer;

class TcpConnection: public small_packages::noncopyable,
    public std::enable_shared_from_this<TcpConnection>{
using readHandlerT = std::function<void(std::string &errMsg)>;
using writeHandlerT = std::function<void(std::string &errMsg)>;
using finishHandlerT = std::function<void(std::string &errMsg,
        std::shared_ptr<TcpConnection>)>;
public:
    //for acceptor
    TcpConnection(EventLoop &loop, int fd);
    //for connector
    TcpConnection(EventLoop &loop, 
            std::unique_ptr<Socket> &socket,
            std::shared_ptr<Event> event);
    ~TcpConnection();
    void AsyncRead(uint64_t expectSize,
            readHandlerT handler);
    void AsyncReadAny(readHandlerT handler);
    void AsyncWrite(writeHandlerT handler);
    void Reset();
    void Finish(std::string &errMsg);
    //FIXME: Buffer split into ReadBuffer && WriteBuffer
    Buffer ReadBuffer_;
    Buffer WriteBuffer_;
private:
    friend class Server;
    friend class Client;
    void InitAccepted(std::string &errMsg);
    void InitConnected(std::string &errMsg);
    void SetFinishHandler(finishHandlerT handler) {
        finishHandler_ = handler;
    }
    void ResetRead() {
        expectSize_ = 0;
        readedSize_ = 0;
    }

    void readHandler();
    void writeHandler();

    uint64_t expectSize_ = 0;
    uint64_t readedSize_ = 0;
    bool anyFlag_ = false;
    readHandlerT userReadHandler_;
    writeHandlerT userWriteHandler_;
    finishHandlerT finishHandler_;

    //composition
    std::unique_ptr<Socket> socket_;
    std::shared_ptr<Event> event_;
    std::shared_ptr<Asyncer> asyncer_;
    std::shared_ptr<small_lock::LockI> lock_;
};
}
}
