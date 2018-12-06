#pragma once

#include <functional>
#include <string>
#include <memory>
#include "event_loop.h"

namespace sheep{
namespace net{
class Event;

//TODO: use with RAII
//thread unsafe
class Asyncer{
using asyncerHandlerT = 
    std::function<void(const std::string &errMsg)>;
public:
    Asyncer(EventLoop &loop);
    ~Asyncer();
    void AsyncDo(asyncerHandlerT handler);
private:
    void cancel();
    EventLoop &loop_;
    asyncerHandlerT handler_;
    std::shared_ptr<Event> event_;
};
}
}
