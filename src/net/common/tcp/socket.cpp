#include "socket.h"

//man socket
#include <sys/types.h>
#include <sys/socket.h>
//man close
#include <unistd.h>
//man inet_addr
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//man htons
#include <arpa/inet.h>
//man fctnl
#include <unistd.h>
#include <fcntl.h>
//man setsockopt
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
//man strerror
#include <cstring>

#include "log.h"

namespace sheep{
namespace net{
Socket::Socket():
    fd_(::socket(AF_INET, SOCK_STREAM, 0)){
    if (fd_ < 0) {
        std::string errMsg;
        formatErrMsg(errMsg, "socket", fd_);
        LOG(FATAL) << errMsg;
        return;
    }
}
Socket::Socket(int fd):
    fd_(fd) {
    if (fd_ < 0) {
        std::string errMsg;
        formatErrMsg(errMsg, "check socket", fd_);
        LOG(FATAL) << errMsg;
        return;
    }
}
Socket::~Socket() {
    if (fd_ != 0) {
        ::close(fd_);
    }
}
void Socket::formatErrMsg(std::string &errMsg,
        const std::string &option,
        int result) {
    errMsg = "call sys " + option + " failed: " + 
        std::strerror(result);
}
void Socket::Bind(std::string &errMsg,
        const std::string &addr, int port) {
    sockaddr_in saddr;
    initSaddr(&saddr, addr, port);
    auto result = ::bind(fd_, (struct sockaddr *)&saddr, 
            sizeof(saddr));
    if (result < 0) {
        formatErrMsg(errMsg, "bind", result);
        return;
    }
}
void Socket::Listen(std::string &errMsg) {
    auto result = ::listen(fd_, BackLog_);
    if (result < 0) {
        formatErrMsg(errMsg, "listen", result);
        return;
    }
}
void Socket::Connect(std::string &errMsg,
        const std::string &addr, int port) {
    addr_ = addr;
    port_ = port;
    sockaddr_in saddr;
    initSaddr(&saddr, addr, port);
    auto result = ::connect(fd_, (struct sockaddr *)&saddr,
            sizeof(saddr));
    if (result < 0) {
        if(errno == EINPROGRESS) {
            return;
        }
        formatErrMsg(errMsg, "connect", errno);
        return;
    }
    return;
}

void Socket::CheckConnect(std::string &errMsg) {
    int error = 0;
    socklen_t len = sizeof(error);
    auto result = getsockopt(fd_, SOL_SOCKET, SO_ERROR,
            &error, &len);
    if (result < 0 || error) {
        if (error) {
            result = error;
        }
        if (!addr_.empty()) {
            formatErrMsg(errMsg, 
                    addr_ + ":" + std::to_string(port_) + 
                    " connected", result);
            return;
        }
        formatErrMsg(errMsg, "getsockopt SO_ERROR", result);
        return;
    }
    return;
}

int Socket::Read(std::string &errMsg, char *buf, int len) {
    auto result = ::read(fd_, buf, len);
    if (result < 0) {
        if (errno == EAGAIN) {
            return result;
        }
        formatErrMsg(errMsg, "read", errno);
        return result;
    }
    return result;
}
int Socket::Write(std::string &errMsg, char *buf, int len) {
    auto result = ::write(fd_, buf, len);
    if (result < 0) {
        if (errno == EAGAIN) {
            return result;
        }
        formatErrMsg(errMsg, "read", errno);
        return result;
    }
    return result;
}
void Socket::initSaddr(void *vaddr, 
        const std::string &addr, int port) {
    auto &saddr = *static_cast<sockaddr_in*>(vaddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if (addr.c_str() != nullptr) {
        saddr.sin_addr.s_addr = ::inet_addr(addr.c_str());
    }else {
        saddr.sin_addr.s_addr = 0;
    }
}
void Socket::SetBlock(std::string &errMsg) {
    SetBlockWithFlag(errMsg, true);
}
void Socket::SetNoblock(std::string &errMsg) {
    SetBlockWithFlag(errMsg, false);
}
void Socket::SetBlockWithFlag(std::string &errMsg, bool block) {
    auto flags = ::fcntl(fd_, F_GETFL);
    if (flags < 0) {
        formatErrMsg(errMsg, "fctnl F_GETFL", flags);
        return;
    }
    if (!block) {
        flags |= O_NONBLOCK;
    }else {
        flags &= ~O_NONBLOCK;
    }
    auto result = ::fcntl(fd_, F_SETFL, flags);
    if (flags < 0) {
        formatErrMsg(errMsg, "fctnl F_SETFL", flags);
        return;
    }
}
void Socket::SetDelay(std::string &errMsg) {
    SetDelayWithFlag(errMsg, true);
}
void Socket::SetNoDelay(std::string &errMsg) {
    SetDelayWithFlag(errMsg, false);
}
void Socket::SetDelayWithFlag(std::string &errMsg, bool delay) {
    int val = 0;
    if (delay) {
        val = 1;
    }
    auto result = ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY,
            &val, sizeof(val));
    if (result < 0) {
        formatErrMsg(errMsg, "setsockopt TCP_NODELAY", errno);
    }
}
void Socket::SetReuseAddr(std::string &errMsg) {
    int yes = 1;
    auto result = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
            &yes, sizeof(yes));
    if (result < 0) {
        formatErrMsg(errMsg, "setsockopt SO_REUSEADDR", result);
    }
}    
void Socket::SetNoTimewait(std::string &errMsg) {
    struct linger so_linger;
    so_linger.l_onoff = true;
    so_linger.l_linger = 0;
    auto result = setsockopt(fd_, SOL_SOCKET, SO_LINGER,
            &so_linger, sizeof(so_linger));
    if (result < 0) {
        formatErrMsg(errMsg, "setsockopt SO_LINGER", result);
    }
}
int Socket::Accept(std::string &errMsg) {
    int newFd = ::accept(fd_, nullptr, nullptr);
    if (newFd < 0) {
        if (errno == EAGAIN) {
            return newFd;
        }
        formatErrMsg(errMsg, "accpet", errno);
    }
    return newFd;
}

void Socket::GetLocalIp(std::string &errMsg, std::string &ip) {
    sockaddr_in local_addr;
    socklen_t len = sizeof(sockaddr);
    auto result = ::getsockname(fd_, (sockaddr*)&local_addr, &len);
    if (result < 0) {
        formatErrMsg(errMsg, "getsockname", result);
        return;
    }
    char addr_buffer[INET_ADDRSTRLEN];
    auto p = ::inet_ntop(AF_INET, &local_addr.sin_addr, 
            addr_buffer, INET_ADDRSTRLEN);
    if (p == nullptr) {
        formatErrMsg(errMsg, "inet_ntop", 1);
        return;
    }
    ip = addr_buffer;
}

void Socket::SetRcvTimeout(std::string &errMsg, uint64_t ms) {
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = ms % 1000 * 1000;
    auto result = setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (result < 0) {
        formatErrMsg(errMsg, "setsockopt SO_LINGER", errno);
    }
}

int Socket::GetFd() {
    return fd_;
}
}
}
