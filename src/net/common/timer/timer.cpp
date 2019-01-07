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
    handler_ = handler;
    asyncer_ = std::make_shared<Asyncer>(loop_);
    std::weak_ptr<Timer> weakThis = shared_from_this();
    asyncer_->AsyncDo([this, weakThis, ms, handler](const std::string &errMsg){
        auto realThis = weakThis.lock();
        if (!realThis) {
            //LOG(WARNING) << "Timer has been destoryed";
            return;
        }
        if (!errMsg.empty()) {
            if (!done_) {
                done_ = true;
                handler(errMsg);
            }
            return;
        }
        //loop thread
        small_lock::UniqueGuard guard(lock_);
        event_ = std::make_shared<Event>(loop_, TimerPollerFactory::Get()->GetPollerType(), 
            UnixTimeMilliSecond() + ms);
        event_->SetReadEvent([this, weakThis, handler](){
            //loop thread
            auto realThis = weakThis.lock();
            if (!realThis) {
                //LOG(WARNING) << "Timer has been destoryed";
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
    if (!done_ && handler_) {
        done_ = true;
        handler_("Timer Canceled");
    }
}
}
}
