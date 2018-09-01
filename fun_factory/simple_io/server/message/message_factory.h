#pragma once
#include "event_loop.h"
#include "message.h"

class MessageFactory {
    public:
        MessageFactory(){};
        virtual ~MessageFactory(){};
        virtual Message* CreateMessage(EventLoop *eventloop) = 0;
};
