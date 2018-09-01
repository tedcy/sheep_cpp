#include "service.h"
#include <assert.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void Service::NextService() {
    if (nextHandler_ != nullptr) {
        nextHandler_();
    }
    delete this;
}

void Service::InitBase(int fd, std::shared_ptr<EventLoop> eventLoop, ServiceNextHandler handler) {
    serverFd_ = fd;
    eventLoop_ = eventLoop;
    nextHandler_ = handler;
}

void Service::InitHandler() {
    eventLoop_->DeleteReadEvent(serverFd_);
    Handler();
}

void Service::AddServerEvent(Option o, std::shared_ptr<BufferWrapper> buffer) {
    std::shared_ptr<service_event::Event> event;
    std::function<void()> startFunc;
    std::function<void()> doneFunc;
    switch (o) {
    case Option::Read:
        cout << "AddServerReadEvent" << endl;
        event = std::make_shared<service_event::Event>(
                service_event::EventType::ServerRead,
                serverFd_, buffer);
        startFunc = [event](){
            cout << "started" << endl;
            event->ServerReadEventHandler();
        };
        doneFunc = [this, event](){
            cout << "finished" << endl;
            eventLoop_->DeleteReadEvent(serverFd_);
            this->eventDoneHander(event);
        };
        event->SetDoneFunc(doneFunc);
        eventLoop_->CreateReadEvent(serverFd_, startFunc);
        break;
    case Option::Write:
        cout << "AddServerWriteEvent" << endl;
        event = std::make_shared<service_event::Event>(
                service_event::EventType::ServerWrite,
                serverFd_, buffer);
        startFunc = [event](){
            cout << "started" << endl;
            event->ServerWriteEventHandler();
        };
        doneFunc = [this, event](){
            cout << "finished" << endl;
            eventLoop_->DeleteWriteEvent(serverFd_);
            this->eventDoneHander(event);
        };
        event->SetDoneFunc(doneFunc);
        eventLoop_->CreateWriteEvent(serverFd_, startFunc);
        break;
    default:
        break;
        //not reach
    }
    events_.insert(event);
}
void Service::AddFileEvent(Option o, 
            std::shared_ptr<BufferWrapper> buffer, std::string &filename) {
}
void Service::AddClientEvent(Option o, 
            std::shared_ptr<BufferWrapper> buffer, std::string &address) {
}
void Service::eventDoneHander(std::shared_ptr<service_event::Event> event) {
    auto setIter = events_.find(event);
    assert(setIter != events_.end());
    events_.erase(event);
    if (event->GetDoneStatus() == service_event::DoneStatus::CLOSED ||
        event->GetDoneStatus() == service_event::DoneStatus::FAILED) {
        close(serverFd_);
        EndHandler();
        delete this;
        return;
    }
    if (events_.empty()) {
        cout << "handler" << endl;
        Handler();
    }
}
