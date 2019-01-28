#pragma once

#include <string>
#include "small_packages.h"

namespace sheep{
namespace net{
class Socket: public small_packages::noncopyable{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();
    //for acceptor
    void Bind(std::string &errMsg,
            const std::string &addr, int port);
    void Listen(std::string &errMsg);
    int Accept(std::string &errMsg);

    //for connector
    void Connect(std::string &errMsg,
            const std::string &addr, int port);
    void CheckConnect(std::string &errMsg);

    //for tcp_connection
    int Read(std::string &errMsg, char* buf, int len);
    int Write(std::string &errMsg, char* buf, int len);
    void GetLocalIp(std::string &errMsg, std::string &ip);

    //ops
    void SetBlock(std::string &errMsg);
    void SetNoblock(std::string &errMsg);
    void SetDelay(std::string &errMsg);
    void SetNoDelay(std::string &errMsg);
    void SetReuseAddr(std::string &errMsg);
    void SetNoTimewait(std::string &errMsg);
    void SetRcvTimeout(std::string &errMsg, uint64_t ms);

    //get members
    int GetFd();
private:
    void formatErrMsg(std::string &errMsg,
            const std::string &option,
            int result);
    void initSaddr(void *vaddr, 
            const std::string &addr, int port);
    void SetBlockWithFlag(std::string &errMsg, bool block);
    void SetDelayWithFlag(std::string &errMsg, bool delay);
    static const int BackLog_ = 522;

    int fd_;
private:
    //error information
    std::string addr_;
    int port_;
};
}
}
