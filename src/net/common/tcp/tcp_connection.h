#pragma once

#include <memory>
#include <functional>
#include "extends/small_packages/small_packages.h"
#include "buffer.h"

namespace sheep{
namespace net{
class Event;
class EventLoop;
class Socket;
class Asyncer;

class TcpConnection: public small_packages::noncopyable,
    public std::enable_shared_from_this<TcpConnection>{
using readHandlerT = std::function<void(const std::string &errMsg)>;
using writeHandlerT = std::function<void(const std::string &errMsg)>;
using finishHandlerT = std::function<void(const std::string &errMsg,
        std::shared_ptr<TcpConnection>&)>;
public:
    //for acceptor
    TcpConnection(EventLoop &loop, int fd);
    //for connector
    TcpConnection(std::unique_ptr<Socket> &socket,
            std::shared_ptr<Event> &event);
    ~TcpConnection();
    void AsyncRead(uint64_t expectSize,
            readHandlerT handler);
    void AsyncReadAny(readHandlerT handler);
    void AsyncWrite(writeHandlerT handler);
    void Reset();
    void Finish(const std::string &errMsg);
    void GetLocalIp(std::string &errMsg, std::string &ip);
    void WriteBufferPush(const char* buf, uint64_t len);
    uint64_t ReadBufferPopHead(char *buf, uint64_t len);
private:
    Buffer ReadBuffer_;
    Buffer WriteBuffer_;
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
    bool finished_ = false; 
    readHandlerT userReadHandler_;
    bool writeDone_ = false;
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
