#include "file_event.h"
#include "common.h"
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
using namespace std;

FileEvent::FileEvent(){
    bzero(this, sizeof(*this));
}

FileEvent::~FileEvent() {
    bzero(this, sizeof(*this));
}

void FileEvent::Init(int epoll_fd, int fd) {
    this->epoll_fd = epoll_fd;
    this->fd = fd;
    this->inited = true;
}

int FileEvent::CreateReadEvent(void (*readCallback)(int fd, void* clientData), void* clientData) {
    if (!this->inited || !readCallback) {
        return SERVER_ERR;
    }
    struct epoll_event event;
    event.events |= EPOLLIN;
    if (this->writeAble) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = this->fd;
    this->readCallback = readCallback;
    this->clientData = clientData;
    if (this->readAble || this->writeAble) {
        this->readAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
    }else {
        this->readAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->fd, &event);
    }
    return SERVER_OK;
}
int FileEvent::CreateWriteEvent(void (*writeCallback)(int fd, void* clientData), void *clientData) {
    if (!this->inited || !writeCallback) {
        return SERVER_ERR;
    }
    struct epoll_event event;
    event.events |= EPOLLOUT;
    if (this->readAble) {
        event.events |= EPOLLIN;
    }
    event.data.fd = this->fd;
    this->writeCallback = writeCallback;
    this->clientData = clientData;
    if (this->readAble || this->writeAble) {
        this->writeAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
    }else {
        this->writeAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->fd, &event);
    }
    return SERVER_OK;
}

bool FileEvent::CheckDoReadEvent() {
    if (!this->inited) {
        return false;
    }
    if (this->readAble) {
        this->readCallback(this->fd, this->clientData);
        return true;
    }
    return false;
}
bool FileEvent::CheckDoWriteEvent() {
    if (!this->inited) {
        return false;
    }
    if (this->writeAble) {
        this->writeCallback(this->fd, this->clientData);
        return true;
    }
    return false;
}
void FileEvent::DeleteReadEvent() {
    if (!this->inited) {
        return;
    }
    struct epoll_event event;
    event.data.fd = this->fd;
    if (this->readAble) {
        struct epoll_event event;
        if (this->writeAble) {
            event.events |= EPOLLOUT;
        }
        event.events &= ~EPOLLIN;
        this->readAble = false;
        this->readCallback = 0;
        epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
        return;
    }
    if (!this->readAble && !this->writeAble) {
        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, this->fd, &event);
    }
    return;
}
void FileEvent::DeleteWriteEvent() {
    if (!this->inited) {
        return;
    }
    struct epoll_event event;
    event.data.fd = this->fd;
    if (this->writeAble) {
        if (this->readAble) {
            event.events |= EPOLLIN;
        }
        event.events &= ~EPOLLOUT;
        this->writeAble = false;
        this->writeCallback = 0;
        epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
        return;
    }
    if (!this->readAble && !this->writeAble) {
        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, this->fd, &event);
    }
    return;
}
