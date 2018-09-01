#pragma once
#include <sys/epoll.h>
#include <functional>

class FileEvent {
using Handler = std::function<void()>;
    public:    
        FileEvent();
        ~FileEvent();
        void Init(int epoll_fd, int fd);
        int CreateReadEvent(Handler readHandler);
        int CreateWriteEvent(Handler writeHandler);
        bool CheckDoReadEvent();
        bool CheckDoWriteEvent();
        void DeleteReadEvent();
        void DeleteWriteEvent();
    private:
        bool readAble = false;
        bool writeAble = false;
        bool inited = false;
        int epoll_fd = 0;
        int fd = 0;
        Handler readHandler = nullptr;
        Handler writeHandler = nullptr;
};
