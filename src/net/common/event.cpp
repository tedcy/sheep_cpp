#include "event.h"
#include "poller.h"
#include "event_loop.h"
#include "log.h"
#include "event_id.h"

namespace sheep{
namespace net{
Event::Event(EventLoop &loop, uint64_t type, int64_t fd):
    loop_(loop),
    poller_(loop.GetPoller(type)), 
    fd_(fd), id_(EventIdCreator::Get()->Create()){
}

Event::~Event() {
    //LOG(DEBUG) << "~Event";
    auto poller = poller_.lock();
    if (!poller) {
        LOG(WARNING) << "poller destroyed";
        return;
    }
    if (readNotify_ || writeNotify_) {
        readNotify_ = false;
        writeNotify_ = false;
        poller->RemoveEvent(this);
    }
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
    if (!readNotify_ && !writeNotify_) {
        poller->RemoveEvent(this);
        return;
    }
    poller->UpdateEvent(shared_from_this());
}

bool Event::GetReadNotify() {
    return readNotify_;
}

bool Event::GetWriteNotify() {
    return writeNotify_;
}

int64_t Event::GetFd() {
    return fd_;
}

int64_t Event::GetId() {
    return id_;
}

void Event::Do() {
    //FIXME stale event
    bool rProcessed = false;
    if (readAble_ && readNotify_) {
        rProcessed = true;
        readCb_();
        readAble_ = false;
    }
    //avoid stale event
    if (!rProcessed && writeAble_ && writeNotify_) {
        writeCb_();
        writeAble_ = false;
    }
}
}
}
