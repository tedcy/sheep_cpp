#pragma once

#include "small_packages.h"
#include "poller.h"
#include <memory>

class EventLoop: public small_packages::noncopyable {
public:
    EventLoop();
    EventLoop(PollerFactory *pollerFactory);
    void Wait();
    void Stop();
private:
friend class Event;
    std::weak_ptr<Poller> GetPoller();
    void loop(); 

    //composition
    std::shared_ptr<Poller> poller_;
    bool stop_ = false;
};
