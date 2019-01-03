#pragma once

#include "poller.h"
#include <sys/epoll.h>
#include <memory>
#include <vector>

namespace sheep{
namespace net{
class Epoller: public Poller{
public:
    Epoller(); 
    std::vector<std::weak_ptr<Event>> Poll(std::string &) override;
    void UpdateEvent(std::shared_ptr<Event> event) override;
    void RemoveEvent(Event *event) override;
private:
    static const uint64_t MaxSize_ = 100000;
    static const uint64_t PollerMaxTime_ = 1;
    //can't use array for avoid including sys/epoll
    std::vector<struct epoll_event> pollEvents_;
    std::vector<std::weak_ptr<Event>> events_;
    int epollfd_ = 0;
};

class EpollerFactory: public PollerFactory {
public:
    uint64_t GetPollerType() override {
        return 0;
    }
    static EpollerFactory* Get() {
        static EpollerFactory pf;
        return &pf;
    }
    std::unique_ptr<Poller> Create() override {
        std::unique_ptr<Poller> p(new Epoller);
        return p;
    }
};
}
}
