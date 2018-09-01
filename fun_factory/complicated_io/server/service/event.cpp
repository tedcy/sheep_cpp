#include "event.h"
#include <unistd.h>
#include <cstring>
#include <memory>
#include <assert.h>
#include <iostream>
using namespace std;

namespace service_event{
Event::Event(EventType type, int fd,
        std::shared_ptr<BufferWrapper> buffer):
    type_(type), fd_(fd), buffer_(buffer){
}

/*
两步创建，因为回调函数可能会调用自己
这个时候自己还是nullptr无法被传参
*/
void Event::SetDoneFunc(EventDoneHandler handler) {
    doneHandler_ = handler;
}

void Event::ServerReadEventHandler() {
    char data[maxSize];
    for (;;) {
        auto leftSize = buffer_->LeftWrite();
        cout << "leftSize\t" << leftSize << endl;
        if (leftSize == 0) {
            doneHandler_();
            return;
        }
        if (leftSize > maxSize) {
            leftSize = maxSize;
        }
        ssize_t n = read(fd_, data, leftSize);
        if (n < 0) {
            if (errno == EAGAIN) {
                return;
            }
            status_ = DoneStatus::FAILED;
            errMsg_ = std::strerror(errno);
            doneHandler_();
            return;
        }else {
            if (n > 0) {
                buffer_->Write(data, n);
            }else {
                status_ = DoneStatus::CLOSED;
                doneHandler_();
                return;
            }
        }
    }
    return;
}
void Event::ServerWriteEventHandler() {
    char data[maxSize];
    for (;;) {
        auto leftSize = buffer_->LeftRead();
        cout << "leftSize\t" << leftSize << endl;
        if (leftSize == 0) {
            doneHandler_();
            return;
        }
        if (leftSize > maxSize) {
            leftSize = maxSize;
        }
        buffer_->Read(data, leftSize);
        ssize_t n = write(fd_, data, leftSize);
        if (n < 0) {
            if (errno == EAGAIN) {
                return;
            }
            status_ = DoneStatus::FAILED;
            errMsg_ = std::strerror(errno);
            doneHandler_();
            return;
        }else {
            if (n > 0) {
                buffer_->AddOffset(n);
            }else {
                //unreach
                assert(false);
            }
        }
    }
    return;
}
void Event::ClientReadEventHandler() {

}
void Event::ClientWriteEventHandler() {

}
void Event::FileReadEventHandler() {

}
void Event::FileWriteEventHandler() {

}
EventType Event::GetEventType() {
    return type_;
}
DoneStatus Event::GetDoneStatus() {
    return status_;
}
}//namespace service_event
