#pragma once
#include "message.h"
#include "event_loop.h"

class RealMessage:public Message {
    public:
        RealMessage(EventLoop *eventLoop):Message(eventLoop) {};
        ~RealMessage() {};
    private:
        ssize_t ReqHeaderLen();
        ssize_t ReqBodyLen(char *recvMsg, int recvLen);
        void Service(char *recvMsg, ssize_t recvLen, char **sendMsg, ssize_t* needSendLen);
};
