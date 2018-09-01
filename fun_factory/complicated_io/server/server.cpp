#include "../common/common.h"
#include "server.h"
#include "service/service.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>//nodelay
#include <cstring>
#include <iostream>
using namespace std;

Server::Server(std::shared_ptr<ServiceFactory> serviceFactory) {
    serviceFactory_ = serviceFactory;
}

Server::~Server() {
}

int Server::Init(char *bindaddr, int port) {
    if (inited_) {
        return SERVER_OK;
    }
    if (eventLoop_->Init(maxSize) == SERVER_ERR) {
        return SERVER_ERR;
    }
    if (tcpServer(bindaddr, port, backlog) < 0) {
        return SERVER_ERR;
    }
    return SERVER_OK;
}

int Server::Serve() {
    return eventLoop_->Run();
}

int Server::tcpServer(char *bindAddr, int port, int backlog) {
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0) {
        return SERVER_ERR;
    }
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if (bindAddr != nullptr) {
        saddr.sin_addr.s_addr = inet_addr(bindAddr);
    }else {
        saddr.sin_addr.s_addr = 0;
    }
    if (listen(listenFd_, (struct sockaddr *)&saddr, sizeof(saddr), backlog) < 0 ) {
        return SERVER_ERR;
    }
    if (setNoTimeWait(listenFd_) < 0) {
        return SERVER_ERR;
    }
    if (setBlock(listenFd_, false) < 0) {
        return SERVER_ERR;
    }
    if (eventLoop_->CreateReadEvent(listenFd_, 
                [this](){
                    this->acceptHandler();
                }) < 0) {
        return SERVER_ERR;
    }
    return listenFd_;
}

int Server::listen(int fd, struct sockaddr *sa, socklen_t sa_len, int backlog) {
    if (bind(fd, sa, sa_len) < 0) {
        close(fd);
        return SERVER_ERR;
    }
    if (::listen(fd, backlog) < 0) {
        close(fd);
        return SERVER_ERR;
    }
    return SERVER_OK;
}

void Server::acceptHandler() {
    cout << "accept start" << endl;
    //struct sockaddr_in addr;
    while(1) {
        //fix 这里如果不用nullptr必须初始化为addr长度否则可能会报invalid argument
        //socklen_t addr_len = sizeof(addr);
        //int newFd = ::accept(listenFd_, (struct sockaddr*)&addr, &addr_len); 
        int newFd = ::accept(listenFd_, nullptr, nullptr); 
        if (newFd < 0) {
            cout << "accept end" << endl;
            if (errno == EAGAIN) {
                return;
            }
            cout << std::strerror(errno) << endl;
            close(listenFd_);
            return;
        }
        setBlock(newFd, false);
        setDelay(newFd, false);
        nextService(newFd);
    }
    return;
}

/*fix lambda表达式会是临时的，当需要在lambda表达式里调用自己的时候
std::functional func;
func = [func]() {
    func()
}
xxx.set(func);
上面的捕获会是一个空值
所以要解除这种自己调用自己的情况，需要把一部分做成非lambda函数
*/
void Server::nextService(int newFd) {
    auto service = serviceFactory_->CreateService();
    service->InitBase(newFd, eventLoop_,
        [this, newFd](){
            this->nextService(newFd);
        });
    eventLoop_->CreateReadEvent(newFd, 
        [service](){
            service->InitHandler();
        });
}

int Server::setBlock(int fd, bool block) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL)) < 0) {
        return SERVER_ERR;
    }
    if (!block) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    if (fcntl(fd, F_SETFL, flags) < 0) {
        return SERVER_ERR;
    }
    return SERVER_OK;
}

int Server::setDelay(int fd, bool delay) {
    int val = 0;
    if (delay) {
        val = 1;
    }
    if (setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&val,sizeof(val)) < 0) {
        return SERVER_ERR;
    }
    return SERVER_OK;
}

//timewait状态不能真正重用
int Server::setReuseAddr(int fd) {
	int yes = 1;                                                                
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {    
        return SERVER_ERR;                                                        
    }                                                                           
    return SERVER_OK;
}

//取消timewait状态来快速重用端口
int Server::setNoTimeWait(int fd) {
    struct linger so_linger;
    so_linger.l_onoff = true;
    so_linger.l_linger = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) == -1) { 
        return SERVER_ERR;                                                        
    }
    return SERVER_OK;
}
