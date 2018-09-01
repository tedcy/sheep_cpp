#include "event_loop.h"
#include "common.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;

EventLoop::EventLoop(){
    bzero(this, sizeof(*this));
}

EventLoop::~EventLoop() {
    if (this->epollfd > 0) {
        close(this->epollfd);
    }
    if (this->events) {
        delete []this->events;
    }
    if (this->fileEvents) {
        delete []this->fileEvents;
    }
    bzero(this, sizeof(*this));
}

int EventLoop::Init(int maxSize) {
    //no use size,see man epoll_create
    this->epollfd = epoll_create(1024);
    if (this->epollfd == -1) {
        return SERVER_ERR;
    }
    this->maxSize = maxSize;
    this->events = new epoll_event[maxSize];
    this->fileEvents = new FileEvent[maxSize];
    for (int i = 0;i < maxSize;i++) {
        this->fileEvents[i].Init(this->epollfd, i);
    }
    this->inited = true;
    return SERVER_OK;
}

int EventLoop::CreateReadEvent(int fd, void (*readCallback)(int fd, void* clientData), void* clientData) {
    if (!this->inited) {
        return SERVER_ERR;
    }
    if (fd > this->maxSize) {
        return SERVER_ERR;
    }
    return this->fileEvents[fd].CreateReadEvent(readCallback, clientData);
}
int EventLoop::CreateWriteEvent(int fd, void (*writeCallback)(int fd, void* clientData), void *clientData) {
    if (!this->inited) {
        return SERVER_ERR;
    }
    if (fd > this->maxSize) {
        return SERVER_ERR;
    }
    return this->fileEvents[fd].CreateWriteEvent(writeCallback, clientData);
}
void EventLoop::DeleteReadEvent(int fd) {
    if (!this->inited) {
        return;
    }
    if (fd > this->maxSize) {
        return;
    }
    this->fileEvents[fd].DeleteReadEvent();
    return;
}
void EventLoop::DeleteWriteEvent(int fd) {
    if (!this->inited) {
        return;
    }
    if (fd > this->maxSize) {
        return;
    }
    this->fileEvents[fd].DeleteWriteEvent();
    return;
}

int EventLoop::Run() {
    int result;
    while(1) {
        result = this->loop();
        if (result != SERVER_OK) {
            return result;
        }
    }
    return SERVER_OK;
}

int EventLoop::loop() {
    int num;
    num = epoll_wait(this->epollfd, this->events, this->maxSize, -1);
    if (num < 0) {
        return SERVER_ERR;
    }
    FileEvent *fileEvent;
    int fd;
    for (int i = 0;i < num;i++) {
        bool readDone = false;
        struct epoll_event *e = this->events + i;
        fd = e->data.fd;
        fileEvent = this->fileEvents + fd;
        if (e->events & EPOLLIN) {
            readDone = fileEvent->CheckDoReadEvent();
        }
        if (e->events & EPOLLOUT || e->events & EPOLLERR || e->events & EPOLLHUP) {
            //stale event
            //!!stale event在ET模式读写同时来怎么办？
            if (!readDone) {
                fileEvent->CheckDoWriteEvent();
            }
        }
    }
    return SERVER_OK;
}
