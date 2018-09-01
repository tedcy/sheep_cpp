#pragma once
#include "file_event.h"
#include <sys/epoll.h>

class EventLoop {
using Handler = std::function<void()>;
public:
    EventLoop();
    ~EventLoop();
    int Init(int maxSize);
    int CreateReadEvent(int fd, Handler readHandler);
    int CreateWriteEvent(int fd, Handler writeHandler);
    void DeleteReadEvent(int fd);
    void DeleteWriteEvent(int fd);
    int Run();
    int loop();
    void Stop();
private:
    int epollfd;
    int maxSize;
    bool inited;
    struct epoll_event* events;
    FileEvent* fileEvents;
};
