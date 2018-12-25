#include "event_loop.h"
#include "event.h"
#include "epoller.h"
#include "timer_poller.h"
#include "asyncer_poller.h"
#include "log.h"
#include <thread>
#include <sstream>

namespace sheep{
namespace net{
unsigned long long g_tid = 0;
EventLoop::EventLoop(){
    AddPoller(EpollerFactory::Get());
    AddPoller(TimerPollerFactory::Get());
    AddPoller(AsyncerPollerFactory::Get());
}

void EventLoop::AddPoller(PollerFactory *pollerFactory) {
    pollers_.insert({pollerFactory->GetPollerType(), pollerFactory->Create()});
}

std::weak_ptr<Poller> EventLoop::GetPoller(uint64_t pollerType) {
    return pollers_[pollerType];
}

void EventLoop::loop() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    std::string stid = oss.str();
    g_tid = std::stoull(stid);
    for (;!stop_;) {
        for (auto &pollerPair: pollers_) {
            doPoller(pollerPair.second); 
        }
    }
}

void EventLoop::doPoller(std::shared_ptr<Poller> &poller) {
    std::string errMsg;
    auto weakEvents = poller->Poll(errMsg);
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg;
        return;
    }
    for (auto &weakEvent: weakEvents) {
        auto event = weakEvent.lock();
        if (!event) {
            LOG(WARNING) << "event has been destoryed(stale event)";
            continue;
        }
        event->Do();
    }
}

void EventLoop::Wait() {
    loop();
}

void EventLoop::Stop() {
    stop_ = true;
}
}
}
