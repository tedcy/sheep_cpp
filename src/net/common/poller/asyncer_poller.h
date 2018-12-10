#pragma once

#include "poller.h"
#include <memory>
#include <list>
#include <unordered_map>
#include "small_packages.h"

namespace sheep{
namespace net{
class AsyncerPoller: public Poller {
public:
    AsyncerPoller();
    std::vector<std::shared_ptr<Event>> Poll(std::string &) override;
    void UpdateEvent(std::shared_ptr<Event> event) override;
    void RemoveEvent(Event* event) override;
private:
    void updateEvent(std::shared_ptr<Event> &event);
    void removeEvent(Event *event);

    using weakEventsT = std::list<std::weak_ptr<Event>>;
    weakEventsT events_;
    //check exists, and save list iter
    std::unordered_map<int64_t, weakEventsT::iterator> eventMap_;
    std::shared_ptr<small_lock::LockI> lock_;
};

class AsyncerPollerFactory: public PollerFactory {
public:
    uint64_t GetPollerType() override {
        return 2;
    }
    static AsyncerPollerFactory* Get() {
        static AsyncerPollerFactory pf;
        return &pf;
    }
    std::unique_ptr<Poller> Create() override {
        std::unique_ptr<Poller> p(new AsyncerPoller);
        return p;
    }
};
}
}
