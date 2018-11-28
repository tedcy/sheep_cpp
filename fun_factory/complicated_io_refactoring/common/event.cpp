#include "event.h"
#include "poller.h"
#include "event_loop.h"
#include "log.h"

Event::Event(EventLoop &loop, int fd):
    poller_(loop.GetPoller()), 
    fd_(fd){
}

Event::~Event() {
    DisableReadNotify();
    DisableWriteNotify();
    update();
}

void Event::SetReadEvent(std::function<void()> cb) {
    readCb_ = cb;
}

void Event::SetWriteEvent(std::function<void()> cb) {
    writeCb_ = cb;
}

void Event::EnableReadNotify() {
    readAble_ = true;
    update();
}

void Event::EnableWriteNotify() {
    writeAble_ = true;
    update();
}

void Event::DisableReadNotify() {
    readAble_ = false;
    update();
}

void Event::DisableWriteNotify() {
    writeAble_ = false;
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

bool Event::GetReadAble() {
    return readAble_;
}

bool Event::GetWriteAble() {
    return writeAble_;
}

int Event::GetFd() {
    return fd_;
}

void Event::Do() {
    //TODO stale event
    if (readAble_) {
        readCb_();
    }
    if (writeAble_) {
        writeCb_();
    }
}
