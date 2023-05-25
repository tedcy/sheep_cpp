#include "timer_poller.h"
#include "event.h"
#include "log.h"
#include "util.h"

namespace sheep{
namespace net{

TimerPoller::TimerPoller():
    lock_(small_lock::MakeLock()){
}

std::vector<std::weak_ptr<Event>> TimerPoller::Poll(std::string &errMsg) {
    std::vector<std::weak_ptr<Event>> events;
    small_lock::UniqueGuard guard(lock_);
    auto now = UnixTimeMilliSecond();
    auto endIter = events_.upper_bound(now);
    //"the internal strucure is destoryed problem"
    //see async_poller commit details
    std::vector<std::weak_ptr<Event>> tmpEvents;
    for (auto iter = events_.begin();iter != endIter;iter++) {
        for (auto &weakEventPair : *iter->second) {
            tmpEvents.push_back(weakEventPair.second);
        }
    }
    for (auto &weakEvent: tmpEvents) {
        auto event = weakEvent.lock();
        if(!event) {
            continue;
        }
        event->SetReadAble();
        events.push_back(event);
    }
    return events;
}

void TimerPoller::UpdateEvent(std::shared_ptr<Event> event) {
    small_lock::UniqueGuard guard(lock_);
    updateEvent(event);
}
void TimerPoller::RemoveEvent(Event *event) {
    small_lock::UniqueGuard guard(lock_);
    removeEvent(event);
}
void TimerPoller::updateEvent(std::shared_ptr<Event> event) {
    auto timeFd = event->GetFd();
    auto id = event->GetId();
    auto iter = eventSet_.find(id);
    //no exists but has flag, should add
    if (iter == eventSet_.end()) {
        eventSet_.insert(id);
        auto eventsIter = events_.find(timeFd);
        std::shared_ptr<std::map<int64_t, std::weak_ptr<Event>>> map;
        if (eventsIter == events_.end()) {
            map = std::make_shared<std::map<int64_t, std::weak_ptr<Event>>>();
            events_.insert({timeFd, map});
        }else {
            map = eventsIter->second;
        }
        map->insert({id, event});
        return;
    }
    //exists and has flag, modify(remove and add)
    removeEvent(event.get());
    updateEvent(event);
}

void TimerPoller::removeEvent(Event *event) {
    auto timeFd = event->GetFd();
    auto id = event->GetId();
    auto eventSetIter = eventSet_.find(id);
    if (eventSetIter == eventSet_.end()) {
        return;
    }
    eventSet_.erase(id);

    auto mapIter = events_.find(timeFd);
    if (mapIter == events_.end()) {
        LOG(FATAL) << "can't reach";
        return;
    }
    auto map = mapIter->second;
    //map has one element, delete map
    if (map->size() == 1) {
        events_.erase(timeFd);
        return;
    }
    //delete element in map
    map->erase(id);
}
}
}
