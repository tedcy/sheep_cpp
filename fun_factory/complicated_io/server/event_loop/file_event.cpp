#include "file_event.h"
#include "../../common/common.h"
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
using namespace std;

FileEvent::FileEvent(){
}

FileEvent::~FileEvent() {
}

void FileEvent::Init(int epoll_fd, int fd) {
    this->epoll_fd = epoll_fd;
    this->fd = fd;
    this->inited = true;
}

int FileEvent::CreateReadEvent(Handler readHandler) {
    if (!this->inited || !readHandler) {
        return SERVER_ERR;
    }
    struct epoll_event event;
    event.events |= EPOLLIN;
    if (this->writeAble) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = this->fd;
    this->readHandler = readHandler;
    if (this->readAble || this->writeAble) {
        this->readAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
    }else {
        this->readAble = true;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->fd, &event);
    }
    return SERVER_OK;
}
int FileEvent::CreateWriteEvent(Handler writeHandler) {
    if (!this->inited || !writeHandler) {
        return SERVER_ERR;
    }
    struct epoll_event event;
    event.events |= EPOLLOUT;
    if (this->readAble) {
        event.events |= EPOLLIN;
    }
    event.data.fd = this->fd;
    this->writeHandler = writeHandler;
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
        this->readHandler();
        return true;
    }
    return false;
}
bool FileEvent::CheckDoWriteEvent() {
    if (!this->inited) {
        return false;
    }
    if (this->writeAble) {
        this->writeHandler();
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
        if (this->writeAble) {
            event.events |= EPOLLOUT;
            event.events &= ~EPOLLIN;
            this->readAble = false;
            this->readHandler = nullptr;
            epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
        }else {
            this->readAble = false;
            this->readHandler = nullptr;
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, this->fd, &event);
        }
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
            event.events &= ~EPOLLOUT;
            this->writeAble = false;
            this->writeHandler = nullptr;
            epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->fd, &event);
        }else {
            this->writeAble = false;
            this->writeHandler = nullptr;
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, this->fd, &event);
        }
    }
    return;
}
