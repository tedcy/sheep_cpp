#pragma once
#include "event_loop.h"
#include "message.h"
#include "real_message.h"
#include "real_message_factory.h"

class RealMessageFactory: public MessageFactory{
    public:
        RealMessageFactory() {};
        ~RealMessageFactory() {};
        Message* CreateMessage(EventLoop *eventloop) {
            Message *m = new RealMessage(eventloop);
            return m;
        }
};
