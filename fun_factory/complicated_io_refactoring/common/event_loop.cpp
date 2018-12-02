#include "event_loop.h"
#include "event.h"
#include "epoller.h"
#include "timer_poller.h"
#include "log.h"

EventLoop::EventLoop(){
    AddPoller(EpollerFactory::Get());
    AddPoller(TimerPollerFactory::Get());
}

void EventLoop::AddPoller(PollerFactory *pollerFactory) {
    pollers_.insert({pollerFactory->GetPollerType(), pollerFactory->Create()});
}

std::weak_ptr<Poller> EventLoop::GetPoller(uint64_t pollerType) {
    return pollers_[pollerType];
}

void EventLoop::loop() {
    for (;!stop_;) {
        for (auto &pollerPair: pollers_) {
            doPoller(pollerPair.second); 
        }
    }
}

void EventLoop::doPoller(std::shared_ptr<Poller> &poller) {
    std::string errMsg;
    auto events = poller->Poll(errMsg);
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg;
        return;
    }
    for (auto &event: events) {
        event->Do();
    }
}

void EventLoop::Wait() {
    loop();
}

void EventLoop::Stop() {
    stop_ = true;
}
