#pragma once

#include <functional>
#include <string>
#include <memory>
#include "net/common/event_loop.h"
#include "net/common/tcp/tcp_connection.h"

namespace sheep{
namespace net{
class Event;
class Asyncer;

class Timer: public std::enable_shared_from_this<Timer>{
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
    bool done_;
    bool used_ = false;
};
}
}
