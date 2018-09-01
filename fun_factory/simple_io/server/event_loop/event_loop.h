#pragma once
#include "file_event.h"
#include <sys/epoll.h>

class EventLoop {
    public:
        EventLoop();
        ~EventLoop();
        int Init(int maxSize);
        int CreateReadEvent(int fd, void (*readCallback)(int fd, void* clientData), void* clientData);
        int CreateWriteEvent(int fd, void (*writeCallback)(int fd, void* clientData), void *clientData);
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
