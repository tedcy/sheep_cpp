#pragma once
#include "event_loop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "message_factory.h"

class Server{
    public:
        Server(MessageFactory *messageFactory);
        ~Server();
        int Init(char *bindaddr, int port);
        int Serve();
    private:
        const static int backlog = 522;
        const static int maxSize = 100000;
        int tcpServer(char* bindaddr, int port, int backlog);//create socket listen
        int listen(int fd, struct sockaddr *sa, socklen_t sa_len, int backlog);//listen and bind
        int accept(int fd);
        static void acceptHandler(int fd, void* clientData);
        void realAcceptHandler(int fd);
        int setBlock(int fd, bool block);
        int setDelay(int fd, bool delay);
        int setReuseAddr(int fd);
        bool inited;
        EventLoop *eventLoop;
        MessageFactory *messageFactory;
};
