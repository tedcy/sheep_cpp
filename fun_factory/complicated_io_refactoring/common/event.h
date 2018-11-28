#pragma once

#include <functional>
#include "small_packages.h"
#include <memory>

class Poller;
class EventLoop;

class Event: public small_packages::noncopyable,
    public std::enable_shared_from_this<Event>{
public:
    Event(EventLoop &loop, int fd);
    ~Event();
    void SetReadEvent(std::function<void()>);
    void SetWriteEvent(std::function<void()>);
    void EnableReadNotify();
    void EnableWriteNotify();
    void DisableReadNotify();
    void DisableWriteNotify();
    bool GetReadAble();
    bool GetWriteAble();
    int GetFd();
    void Do();

private:
    void update();
    //associated
    std::weak_ptr<Poller> poller_;

    //composition
    int fd_ = 0;
    bool readAble_ = false;
    bool writeAble_ = false;
    std::function<void()> readCb_;
    std::function<void()> writeCb_;
};
