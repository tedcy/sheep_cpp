#include "timer.h"
#include "event.h"
#include "log.h"
#include "timer_poller.h"

namespace sheep{
namespace net{
Timer::Timer(EventLoop &loop) :
    loop_(loop){
}

void Timer::AsyncWait(uint64_t ms, timerHandlerT handler) {
    Cancel();
    handler_ = handler;
    event_ = std::make_shared<Event>(loop_, TimerPollerFactory::Get()->GetPollerType(), 
            UnixTimeMilliSecond() + ms);
    std::weak_ptr<Event> weakEvent = event_;
    event_->SetReadEvent([weakEvent, this](){
        auto event = weakEvent.lock();
        if (!event) {
            LOG(WARNING) << "Timer has been destoryed";
            return;
        }
        handler_("");
    });
    event_->EnableReadNotify();
}

void Timer::Cancel() {
    if (event_ == nullptr) {
        return;
    }
    event_->DisableReadNotify();
    handler_("Timer Canceled");
}
}
}
