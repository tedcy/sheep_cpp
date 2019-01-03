#include "timer.h"
#include "event.h"
#include "log.h"
#include "timer_poller.h"
#include "asyncer.h"

namespace sheep{
namespace net{
Timer::Timer(EventLoop &loop) :
    loop_(loop) ,
    lock_(small_lock::MakeRecursiveLock()){
}

Timer::~Timer() {
    if (!done_ && handler_) {
        handler_("Timer Canceled");
    }
}

//any thread
//asyncer_,event_,handler_ add lock
void Timer::AsyncWait(uint64_t ms, timerHandlerT handler) {
    small_lock::UniqueGuard guard(lock_);
    if (used_) {
        handler("Timer can't reuse");
        return;
    }
    used_ = true;
    done_ = false;
    cancel();
    asyncer_ = std::make_shared<Asyncer>(loop_);
    asyncer_->AsyncDo([this, ms, handler](const std::string &errMsg){
        if (!errMsg.empty()) {
            if (!done_) {
                done_ = true;
                handler(errMsg);
            }
            return;
        }
        //loop thread
        small_lock::UniqueGuard guard(lock_);
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
            if (!done_) {
                done_ = true;
                handler_("");
            }
        });
        event_->EnableReadNotify();
    });
}

void Timer::Cancel() {
    //any thread
    small_lock::UniqueGuard guard(lock_);
    cancel();
}

void Timer::cancel() {
    if (asyncer_ != nullptr) {
        asyncer_->Cancel();
    }
    if (event_ == nullptr) {
        return;
    }
    event_ ->DisableReadNotify();
    if (!done_) {
        done_ = true;
        handler_("Timer Canceled");
    }
}
}
}
