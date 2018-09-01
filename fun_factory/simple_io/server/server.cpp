#include "server.h"
#include "common.h"
#include "event_loop.h"
#include "message_factory.h"
#include "message.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>//nodelay
#include <iostream>
using namespace std;

Server::Server(MessageFactory *messageFactory) {
    bzero(this, sizeof(*this));
    this->messageFactory = messageFactory;
}

Server::~Server() {
    delete this->eventLoop;
    bzero(this, sizeof(*this));
}

int Server::Init(char *bindaddr, int port) {
    if (this->inited) {
        return SERVER_OK;
    }
    this->eventLoop = new EventLoop;
    if (this->eventLoop->Init(maxSize) == SERVER_ERR) {
        return SERVER_ERR;
    }
    if (this->tcpServer(bindaddr, port, backlog) < 0) {
        return SERVER_ERR;
    }
    return SERVER_OK;
}

int Server::Serve() {
    return this->eventLoop->Run();
}

int Server::tcpServer(char *bindAddr, int port, int backlog) {
    int listenFd;
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        return SERVER_ERR;
    }
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    if (bindAddr != 0) {
        saddr.sin_addr.s_addr = inet_addr(bindAddr);
    }else {
        saddr.sin_addr.s_addr = 0;
    }
    if (this->listen(listenFd, (struct sockaddr *)&saddr, sizeof(struct sockaddr), backlog) < 0 ) {
        return SERVER_ERR;
    }
    if (this->setBlock(listenFd, false) < 0) {
        return SERVER_ERR;
    }
    if (this->eventLoop->CreateReadEvent(listenFd, Server::acceptHandler, this) < 0) {
        return SERVER_ERR;
    }
    return listenFd;
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

void Server::acceptHandler(int fd, void* clientData) {
    Server *s = (Server*)clientData;
    s->realAcceptHandler(fd);
}

void Server::realAcceptHandler(int fd) {
    struct sockaddr addr;
    while(1) {
        socklen_t addr_len;
        int newFd = ::accept(fd, &addr, &addr_len); 
        if (newFd < 0) {
            if (errno == EAGAIN) {
                return;
            }
            close(newFd);
            return;
        }
        this->setBlock(newFd, false);
        this->setDelay(newFd, false);
        Message *m = this->messageFactory->CreateMessage(this->eventLoop);
        this->eventLoop->CreateReadEvent(newFd, Message::ReadEventHandler, (void*)m);
    }
    return;
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

int Server::setReuseAddr(int fd) {
	int yes = 1;                                                                
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {    
        return SERVER_ERR;                                                        
    }                                                                           
    return SERVER_OK;
}
