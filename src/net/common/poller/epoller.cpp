#include "epoller.h"

#include "event.h"
#include "log.h"

namespace sheep{
namespace net{
Epoller::Epoller() :
    pollEvents_(MaxSize_),
    events_(MaxSize_),
    epollfd_(::epoll_create(1000)){
}

std::vector<std::weak_ptr<Event>> Epoller::Poll(
        std::string &errMsg) {
    std::vector<std::weak_ptr<Event>> events;
    int num;
    num = ::epoll_wait(epollfd_, &(*pollEvents_.begin()),
            MaxSize_, PollerMaxTime_);
    if (num < 0) {
        errMsg = "epoll_wait returns " + std::to_string(num);
        return events;
    }
    int i = 0;
    for (auto &pollEvent : pollEvents_) {
        if (i == num) {
            break;
        }
        auto fd = pollEvent.data.fd;
        auto weakEvent = events_[fd];
        auto event = weakEvent.lock();
        if (!event) {
            LOG(WARNING) << "epoll event has been destoryed";
            continue;
        }
        if (pollEvent.events & EPOLLIN) {
            event->SetReadAble();
            //LOG(DEBUG) << event->GetFd() << " readable";
        }else {
            if (pollEvent.events & EPOLLOUT || 
                pollEvent.events & EPOLLERR ||
                pollEvent.events & EPOLLHUP) {
                //TODO add event errable
                event->SetWriteAble();
                //LOG(DEBUG) << event->GetFd() << " writeable";
            }else {
                LOG(FATAL) << "can't access here " << pollEvent.events;
            }
        }
        events.push_back(event);
        i++;
    }
    return events;
}

void Epoller::UpdateEvent(std::shared_ptr<Event> event) {
    struct epoll_event epollEvent = {0};
    if (event->GetReadNotify()) {
        epollEvent.events |= EPOLLIN;
    }
    if (event->GetWriteNotify()) {
        epollEvent.events |= EPOLLOUT;
    }
    int fd = event->GetFd();
    epollEvent.data.fd = fd;
    //no exists but has flag, should add
    if (events_[fd].lock() == nullptr) {
        events_[fd] = event;
        //LOG(DEBUG) << "add " << fd << event->GetReadNotify()
        //   << event->GetWriteNotify(); 
        ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &epollEvent);
        return;
    }
    //LOG(DEBUG) << "modify " << fd << event->GetReadNotify()
    //       << event->GetWriteNotify(); 
    //exists and has flag, modify
    ::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &epollEvent);
}

void Epoller::RemoveEvent(Event *event) {
    auto fd = event->GetFd();
    struct epoll_event epollEvent = {0};
    epollEvent.data.fd = fd;
    //LOG(DEBUG) << "remove " << fd;
    //when called, event can be not destoryed
    //but need to clean read and write flag
    events_[fd] = std::weak_ptr<Event>();
    ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &epollEvent);
    return;
}
}
}
