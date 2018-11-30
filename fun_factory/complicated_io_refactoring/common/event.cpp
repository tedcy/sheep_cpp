#include "event.h"
#include "poller.h"
#include "event_loop.h"
#include "log.h"

Event::Event(EventLoop &loop, int fd):
    poller_(loop.GetPoller()), 
    fd_(fd){
}

Event::~Event() {
    remove(fd_);
}

void Event::SetReadEvent(std::function<void()> cb) {
    readCb_ = cb;
}

void Event::SetWriteEvent(std::function<void()> cb) {
    writeCb_ = cb;
}

void Event::EnableReadNotify() {
    readNotify_ = true;
    update();
}

void Event::EnableWriteNotify() {
    writeNotify_ = true;
    update();
}

void Event::DisableReadNotify() {
    readNotify_ = false;
    update();
}

void Event::DisableWriteNotify() {
    writeNotify_ = false;
    update();
}

void Event::update() {
    auto poller = poller_.lock();
    if (!poller) {
        LOG(WARNING) << "poller destroyed";
        return;
    }
    poller->UpdateEvent(shared_from_this());
}

void Event::remove(int fd) {
    auto poller = poller_.lock();
    if (!poller) {
        LOG(WARNING) << "poller destroyed";
        return;
    }
    poller->RemoveEvent(fd);
}

bool Event::GetReadNotify() {
    return readNotify_;
}

bool Event::GetWriteNotify() {
    return writeNotify_;
}

int Event::GetFd() {
    return fd_;
}

void Event::Do() {
    //TODO stale event
    if (readAble_ && readNotify_) {
        readCb_();
        readAble_ = false;
    }
    if (writeAble_ && writeNotify_) {
        writeCb_();
        writeAble_ = false;
    }
}
