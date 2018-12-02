#include "timer_poller.h"
#include "small_timer_factory.h"
#include "event.h"
#include "log.h"

TimerPoller::TimerPoller() {
}

std::vector<std::shared_ptr<Event>> TimerPoller::Poll(std::string &errMsg) {
    std::vector<std::shared_ptr<Event>> events;
    auto now = small_timer::UnixTimeMilliSecond();
    auto endIter = events_.upper_bound(now);
    for (auto iter = events_.begin();iter != endIter;iter++) {
        for (auto &weakEventPair : *iter->second) {
            auto event = weakEventPair.second.lock();
            if(!event) {
                LOG(WARNING) << "event has been destoryed";
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
    RemoveEvent(event);
    UpdateEvent(event);
}

//TODO now to test
void TimerPoller::RemoveEvent(std::shared_ptr<Event> event) {
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
