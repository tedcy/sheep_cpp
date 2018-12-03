#pragma once

#include "small_packages.h"
#include "poller.h"
#include <memory>
#include <map>

class EventLoop: public small_packages::noncopyable {
public:
    EventLoop();
    void AddPoller(PollerFactory *pollerFactory);
    void Wait();
    void Stop();
private:
friend class Event;
    std::weak_ptr<Poller> GetPoller(uint64_t pollerType);
    void loop(); 
    void doPoller(std::shared_ptr<Poller> &poller);

    //composition
    std::map<uint64_t, std::shared_ptr<Poller>> pollers_;
    bool stop_ = false;
};
