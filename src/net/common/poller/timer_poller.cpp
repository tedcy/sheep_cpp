#include "timer_poller.h"
#include "event.h"
#include "log.h"

namespace sheep{
namespace net{
uint64_t UnixTimeSecond() {
    std::chrono::time_point<std::chrono::system_clock,std::chrono::seconds> tp =
        std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return uint64_t(timestamp);
}
uint64_t UnixTimeMilliSecond() {
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return uint64_t(timestamp);
}

TimerPoller::TimerPoller() {
}

std::vector<std::weak_ptr<Event>> TimerPoller::Poll(std::string &errMsg) {
    std::vector<std::weak_ptr<Event>> events;
    auto now = UnixTimeMilliSecond();
    auto endIter = events_.upper_bound(now);
    for (auto iter = events_.begin();iter != endIter;iter++) {
        for (auto &weakEventPair : *iter->second) {
            auto event = weakEventPair.second.lock();
            if(!event) {
                LOG(WARNING) << "timer event has been destoryed";
                continue;
            }
            event->SetReadAble();
            events.push_back(event);
        }
    }
    return events;
}

void TimerPoller::UpdateEvent(std::shared_ptr<Event> event) {
    auto timeFd = event->GetFd();
    auto id = event->GetId();
    auto iter = eventSet_.find(id);
    //no exists but has flag, should add
    if (iter == eventSet_.end()) {
        eventSet_.insert(id);
        auto map = std::make_shared<std::map<int64_t, std::weak_ptr<Event>>>();
        map->insert({id, event});
        events_.insert({timeFd, map});
        return;
    }
    //exists and has flag, modify(remove and add)
    RemoveEvent(event.get());
    UpdateEvent(event);
}

void TimerPoller::RemoveEvent(Event *event) {
    auto timeFd = event->GetFd();
    auto id = event->GetId();
    auto map = events_[timeFd];
    //map has one element, delete map
    if (map->size() == 1) {
        events_.erase(timeFd);
        eventSet_.erase(id);
        return;
    }
    //delete element in map
    map->erase(id);
}
}
}
