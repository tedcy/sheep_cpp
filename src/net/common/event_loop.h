#pragma once

#include "extends/small_packages/small_packages.h"
#include "poller.h"
#include <memory>
#include <map>

namespace sheep{
namespace net{
class EventLoop: public small_packages::noncopyable {
public:
    EventLoop();
    void Wait();
    void Stop();
    void runOnce();
private:
friend class Event;
    void AddPoller(PollerFactory *pollerFactory);
    std::weak_ptr<Poller> GetPoller(uint64_t pollerType);
    void loop(); 
    void doPoller(std::shared_ptr<Poller> &poller);

    //composition
    std::map<uint64_t, std::shared_ptr<Poller>> pollers_;
    bool stop_ = false;
};
}
}
