#pragma once
#include <sys/epoll.h>

class FileEvent {
    public:    
        FileEvent();
        ~FileEvent();
        void Init(int epoll_fd, int fd);
        int CreateReadEvent(void (*readCallback)(int fd, void* clientData), void* clientData);
        int CreateWriteEvent(void (*readCallback)(int fd, void* clientData), void *clientData);
        bool CheckDoReadEvent();
        bool CheckDoWriteEvent();
        void DeleteReadEvent();
        void DeleteWriteEvent();
    private:
        bool readAble;
        bool writeAble;
        bool inited;
        int epoll_fd;
        int fd;
        void* clientData;
        void (*readCallback)(int fd, void* clientData);
        void (*writeCallback)(int fd, void* clientData);
};
