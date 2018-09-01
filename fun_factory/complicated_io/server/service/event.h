#pragma once
#include <memory>
#include <functional>
#include "../buffer/buffer.h"
#include "../../common/common.h"
namespace service_event{
enum class EventType {
    ServerRead,
    ServerWrite,
    ClientRead,
    ClientWrite,
    FileRead,
    FileWrite
};
enum class DoneStatus {
    OK,
    FAILED,
    CLOSED
};
class Event {
using EventDoneHandler = std::function<void()>;
public:
    Event(EventType type, int fd,
            std::shared_ptr<BufferWrapper> buffer);
    ~Event() = default;
    void SetDoneFunc(EventDoneHandler doneHandler_);
    EventType GetEventType();
    DoneStatus GetDoneStatus();
    void ServerReadEventHandler();
    void ServerWriteEventHandler();
    void ClientReadEventHandler();
    void ClientWriteEventHandler();
    void FileReadEventHandler();
    void FileWriteEventHandler();
private:
    EventType type_;
    DoneStatus status_;
    std::string errMsg_;
    int fd_;
    std::shared_ptr<BufferWrapper> buffer_;
    EventDoneHandler doneHandler_;
    static const int maxSize = 4 * 1024;
};
}//namespace service_event
