#pragma once

#include <functional>
#include <string>
#include <memory>
#include "event_loop.h"

namespace sheep{
namespace net{
class Event;

class Timer{
using timerHandlerT = 
    std::function<void(const std::string &errMsg)>;
public:
    Timer(EventLoop &loop);
    void AsyncWait(uint64_t ms, timerHandlerT handler);
    void Cancel();
private:
    EventLoop &loop_;
    timerHandlerT handler_;
    std::shared_ptr<Event> event_;
};
}
}
