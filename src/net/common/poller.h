#pragma once

#include <vector>
#include <memory>
#include "extends/small_packages/small_packages.h"

namespace sheep{
namespace net{
class Event;

class Poller: public small_packages::noncopyable {
public:
    virtual ~Poller() = default;
    virtual std::vector<std::weak_ptr<Event>> Poll(std::string&) = 0;
    virtual void UpdateEvent(std::shared_ptr<Event> event) = 0;
    virtual void RemoveEvent(Event *event) = 0;
};

class PollerFactory: public small_packages::noncopyable {
public:
    virtual ~PollerFactory() = default;
    virtual uint64_t GetPollerType() = 0;
    virtual std::unique_ptr<Poller> Create() = 0;
};
}
}
