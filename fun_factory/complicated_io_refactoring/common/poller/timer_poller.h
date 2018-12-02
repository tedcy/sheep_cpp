#pragma once

#include "poller.h"
#include <memory>
#include <vector>
#include <map>
#include <set>

class TimerPoller: public Poller {
public:
    TimerPoller(); 
    std::vector<std::shared_ptr<Event>> Poll(std::string &) override;
    void UpdateEvent(std::shared_ptr<Event> event) override;
    void RemoveEvent(std::shared_ptr<Event> event) override;
private:
    //<timeFd, id, event>
    std::map<int64_t, 
        std::shared_ptr<
            std::map<int64_t, std::weak_ptr<Event>>
            >
            > events_;
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
