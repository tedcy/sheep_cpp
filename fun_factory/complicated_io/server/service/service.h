#pragma once
#include <memory>
#include <set>
#include <string>
#include <functional>
#include "event.h"
#include "../buffer/buffer.h"
#include "../../common/common.h"
#include "../event_loop/event_loop.h"
#include <iostream>
class Service {
using ServiceNextHandler = std::function<void()>;
public:
    Service() = default;
    virtual ~Service() {
        std::cout << "deleted" << std::endl;
    }
    void InitBase(int fd, std::shared_ptr<EventLoop> eventLoop, ServiceNextHandler handler);
    void AddServerEvent(Option o, std::shared_ptr<BufferWrapper> buffer);
    void AddFileEvent(Option o, 
            std::shared_ptr<BufferWrapper> buffer, std::string &filename);
    void AddClientEvent(Option o, 
            std::shared_ptr<BufferWrapper> buffer, std::string &address);
    void NextService();
    void InitHandler();
private:
    ServiceNextHandler nextHandler_ = nullptr;
    void eventDoneHander(std::shared_ptr<service_event::Event> event);
    virtual void Handler() = 0;
    virtual void EndHandler() {};
    std::set<std::shared_ptr<service_event::Event>> events_;
    std::shared_ptr<EventLoop> eventLoop_;
    int serverFd_ = 0;
};
