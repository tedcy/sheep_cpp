#include "timer.h"
#include "event.h"
#include "log.h"
#include "timer_poller.h"
#include "asyncer.h"

namespace sheep{
namespace net{
Timer::Timer(EventLoop &loop) :
    loop_(loop) ,
    lock_(small_lock::MakeLock()){
}

Timer::~Timer() {
}

//any thread
//asyncer_,event_,handler_ add lock
void Timer::AsyncWait(uint64_t ms, timerHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    asyncer_ = std::make_shared<Asyncer>(loop_);
    asyncer_->AsyncDo([this, ms, handler](const std::string &errMsg){
        if (!errMsg.empty()) {
            handler_(errMsg);
            return;
        }
        //loop thread
        small_lock::UniqueGuard guard(lock_);
        cancel();
        handler_ = handler;
        event_ = std::make_shared<Event>(loop_, TimerPollerFactory::Get()->GetPollerType(), 
            UnixTimeMilliSecond() + ms);
        std::weak_ptr<Event> weakEvent = event_;
        event_->SetReadEvent([weakEvent, this](){
            //loop thread
            auto event = weakEvent.lock();
            if (!event) {
                LOG(WARNING) << "Timer has been destoryed";
                return;
            }
            small_lock::UniqueGuard guard(lock_);
            event_->DisableReadNotify();
            handler_("");
        });
        event_->EnableReadNotify();
        asyncer_ = nullptr;
    });
}

void Timer::Cancel() {
    //any thread
    small_lock::UniqueGuard guard(lock_);
    cancel();
}

void Timer::cancel() {
    asyncer_->Cancel();
    if (event_ == nullptr) {
        return;
    }
    event_ ->DisableReadNotify();
    handler_("Timer Canceled");
}
}
}
