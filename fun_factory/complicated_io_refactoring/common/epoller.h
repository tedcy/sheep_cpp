#pragma once

#include "poller.h"
#include <memory>
#include <vector>

class Epoller: public Poller{
public:
    Epoller(); 
    std::vector<std::shared_ptr<Event>> Poll(std::string &) override;
    void UpdateEvent(std::shared_ptr<Event> event) override;
private:
    static const uint64_t MaxSize_ = 100000;
    //can't use array for avoid including sys/epoll
    std::vector<struct epoll_event> pollEvents_;
    std::vector<std::shared_ptr<Event>> events_;
    int epollfd_;
};

class EpollerFactory: public PollerFactory {
public:
    static EpollerFactory* Get() {
        static EpollerFactory pf;
        return &pf;
    }
    std::unique_ptr<Poller> Create() override {
        std::unique_ptr<Poller> p(new Epoller);
        return p;
    }
};
