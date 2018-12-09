#pragma once

#include <functional>
#include <string>
#include <memory>
#include "event_loop.h"
#include "small_packages.h"

namespace sheep{
namespace net{
class Event;
class Asyncer;

class Timer{
using timerHandlerT = 
    std::function<void(const std::string &errMsg)>;
public:
    Timer(EventLoop &loop);
    ~Timer();
    void AsyncWait(uint64_t ms, timerHandlerT handler);
    void Cancel();
private:
    void cancel();
    EventLoop &loop_;
    timerHandlerT handler_;
    std::shared_ptr<Event> event_;
    std::shared_ptr<Asyncer> asyncer_;
    std::shared_ptr<small_lock::LockI> lock_;
};
}
}
