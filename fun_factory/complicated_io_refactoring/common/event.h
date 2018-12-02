#pragma once

#include <functional>
#include "small_packages.h"
#include <memory>

class Poller;
class EventLoop;

class Event: public small_packages::noncopyable,
    public std::enable_shared_from_this<Event>{
public:
    Event(EventLoop &loop, uint64_t type, int64_t fd);
    ~Event();
    void Clean();
    void SetReadEvent(std::function<void()>);
    void SetWriteEvent(std::function<void()>);
    void EnableReadNotify();
    void EnableWriteNotify();
    void DisableReadNotify();
    void DisableWriteNotify();
    bool GetReadNotify();
    bool GetWriteNotify();
    void SetReadAble() {
        readAble_ = true;
    }
    void SetWriteAble() {
        writeAble_ = true;
    }
    int64_t GetFd();
    int64_t GetId();
    void Do();

private:
    void update();
    //associated
    std::weak_ptr<Poller> poller_;

    //composition
    //not only one
    int64_t fd_ = 0;
    //only one
    int64_t id_ = 0;
    bool readAble_ = false;
    bool writeAble_ = false;
    bool readNotify_ = false;
    bool writeNotify_ = false;
    std::function<void()> readCb_;
    std::function<void()> writeCb_;
};
