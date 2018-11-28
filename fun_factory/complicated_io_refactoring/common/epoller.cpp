#include "epoller.h"

#include "event.h"
#include "log.h"
#include <sys/epoll.h>

Epoller::Epoller() :
    pollEvents_(MaxSize_),
    events_(MaxSize_),
    epollfd_(::epoll_create(1000)){
}

std::vector<std::shared_ptr<Event>> Epoller::Poll(
        std::string &errMsg) {
    std::vector<std::shared_ptr<Event>> events;
    int num;
    num = ::epoll_wait(epollfd_, &(*pollEvents_.begin()),
            MaxSize_, -1);
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
        auto event = events_[fd];
        events.push_back(event);
        i++;
    }
    return events;
}

void Epoller::UpdateEvent(std::shared_ptr<Event> event) {
    struct epoll_event epollEvent;
    if (event->GetReadAble()) {
        epollEvent.events |= EPOLLIN;
    }
    if (event->GetWriteAble()) {
        epollEvent.events |= EPOLLOUT;
    }
    int fd = event->GetFd();
    epollEvent.data.fd = fd;
    //no flag, should remove
    if (epollEvent.events == 0) {
        if (events_[fd] == nullptr) {
            LOG(FATAL) << "wtf bug";
        }
        events_[fd] = nullptr;
        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &epollEvent);
        return;
    }
    //no exists but has flag, should add
    if (events_[fd] == nullptr) {
        events_[fd] = event;
        ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &epollEvent);
        return;
    }
    //exists and has flag, modify
    ::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &epollEvent);
}
