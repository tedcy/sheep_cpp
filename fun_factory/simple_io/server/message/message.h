#pragma once
#include "event_loop.h"


class Message {
    public:
        Message(EventLoop *eventLoop);
        virtual ~Message();
        static void ReadEventHandler(int fd, void* clientData);
        void realReadEventHandler(int fd);
    private:
        const static int maxSize = 128*1024;
        EventLoop *eventLoop;
        //recv req
        virtual ssize_t ReqHeaderLen() = 0;
        virtual ssize_t ReqBodyLen(char *recvMsg, int recvLen) = 0;
        virtual void Service(char *recvMsg, ssize_t recvLen, char **sendMsg, ssize_t* needSendLen) = 0;
        //todo buffer pool化
        bool parsedHeader;
        bool parsedBody;
        char *recvMsg;
        ssize_t recvLen;
        ssize_t needRecvLen;

        //send resp
        char *sendMsg;
        ssize_t sendLen;
        ssize_t needSendLen;
        static void writeEventHandler(int fd, void* clientData);
        void realWriteEventHandler(int fd);
};
