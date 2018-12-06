#include "asyncer_poller.h"
#include "event.h"
#include "log.h"

namespace sheep{
namespace net{
AsyncerPoller::AsyncerPoller() :
    lock_(small_lock::MakeLock()){
}
std::vector<std::shared_ptr<Event>> AsyncerPoller::Poll(std::string &) {
    std::vector<std::shared_ptr<Event>> events;
    small_lock::UniqueGuard guard(lock_);
    for(auto &weakEvent: events_) {
        auto event = weakEvent.lock();
        if (!event) {
            LOG(WARNING) << "async event has been destoryed";
            continue;
        }
        event->SetReadAble();
        events.push_back(event);
    }
    return events;
}
void AsyncerPoller::UpdateEvent(std::shared_ptr<Event> event) {
    small_lock::UniqueGuard guard(lock_);
    updateEvent(event);
}
void AsyncerPoller::RemoveEvent(std::shared_ptr<Event> event) {
    small_lock::UniqueGuard guard(lock_);
    removeEvent(event);
}

//O(1)
void AsyncerPoller::updateEvent(std::shared_ptr<Event> &event) {
    auto id = event->GetId();
    auto iter = eventMap_.find(id);
    //not exist, insert
    if (iter == eventMap_.end()) {
        std::weak_ptr<Event> weakEvent = event;
        events_.push_back(weakEvent);
        auto riter = events_.rbegin();
        auto iter = riter.base();
        eventMap_.insert({id, --iter});
        return;
    }
    //exist. earse and insert
    removeEvent(event);
    updateEvent(event);
}
//O(1)
void AsyncerPoller::removeEvent(std::shared_ptr<Event> &event) {
    auto id = event->GetId();
    auto iter = eventMap_[id];
    //iter must exist
    events_.erase(iter);
    eventMap_.erase(id);
}
}
}
