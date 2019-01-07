#pragma once

#include "poller.h"
#include <memory>
#include <vector>
#include <map>
#include <set>
#include "small_packages.h"

namespace sheep{
namespace net{
uint64_t UnixTimeSecond();
uint64_t UnixTimeMilliSecond();

class TimerPoller: public Poller {
public:
    TimerPoller(); 
    std::vector<std::weak_ptr<Event>> Poll(std::string &) override;
    void UpdateEvent(std::shared_ptr<Event> event) override;
    void RemoveEvent(Event *event) override;
private:
    void updateEvent(std::shared_ptr<Event> event);
    void removeEvent(Event *event);
    std::shared_ptr<small_lock::LockI> lock_;
    //<timeFd, id, event>
    std::map<int64_t, 
        std::shared_ptr<
            std::map<int64_t, std::weak_ptr<Event>>
            >
            > events_;
    //check if exists
    std::set<int64_t> eventSet_;
};

class TimerPollerFactory: public PollerFactory {
public:
    uint64_t GetPollerType() override {
        return 1;
    }
    static TimerPollerFactory* Get() {
        static TimerPollerFactory pf;
        return &pf;
    }
    std::unique_ptr<Poller> Create() override {
        std::unique_ptr<Poller> p(new TimerPoller);
        return p;
    }
};
}
}
