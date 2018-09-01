#pragma once
#include "event_loop/event_loop.h"
#include "service/service_factory.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <memory>

class Server{
    public:
        Server(std::shared_ptr<ServiceFactory> serviceFactory);
        ~Server();
        int Init(char *bindaddr, int port);
        int Serve();
    private:
        const static int backlog = 522;
        const static int maxSize = 100000;
        int tcpServer(char* bindaddr, int port, int backlog);//create socket listen
        int listen(int fd, struct sockaddr *sa, socklen_t sa_len, int backlog);//listen and bind
        int accept(int fd);
        void acceptHandler();
        void nextService(int newFd);
        int setBlock(int fd, bool block);
        int setDelay(int fd, bool delay);
        int setReuseAddr(int fd);
        int setNoTimeWait(int fd);
        int listenFd_ = -1;
        bool inited_ = false;
        std::shared_ptr<EventLoop> eventLoop_ = std::make_shared<EventLoop>();
        std::shared_ptr<ServiceFactory> serviceFactory_;
};
