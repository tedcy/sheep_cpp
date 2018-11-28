#include "event_loop.h"
#include "event.h"
#include "epoller.h"
#include "log.h"

EventLoop::EventLoop() :
    poller_(EpollerFactory::Get()->Create()){
}

EventLoop::EventLoop(PollerFactory *pollerFactory) :
    poller_(pollerFactory->Create()){
}

std::weak_ptr<Poller> EventLoop::GetPoller() {
    return poller_;
}

void EventLoop::loop() {
    for (;!stop_;) {
        std::string errMsg;
        auto events = poller_->Poll(errMsg);
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            continue;
        }
        for (auto &event: events) {
            event->Do();
        }
    }
}

void EventLoop::Wait() {
    loop();
}

void EventLoop::Stop() {
    stop_ = true;
}
