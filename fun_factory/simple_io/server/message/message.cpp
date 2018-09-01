#include "message.h"
#include "event_loop.h"
#include <unistd.h>
#include <errno.h>
#include <iostream>
using namespace std;

Message::Message(EventLoop *eventLoop) {
    this->eventLoop = eventLoop;
    this->recvMsg = new char[maxSize];
}

Message::~Message() {
    delete []this->recvMsg;
}

void Message::ReadEventHandler(int fd, void *clientData) {
    cout << "do read handler " << fd << endl;
    Message *m = (Message*)clientData;
    m->realReadEventHandler(fd);
}

void Message::realReadEventHandler(int fd) {
    if (!this->parsedHeader) {
        cout << "header start" << endl;
        int headerLen = this->ReqHeaderLen();
        ssize_t n = read(fd, recvMsg + this->recvLen, size_t(headerLen - this->recvLen));
        if (n < 0) {
            if (errno == EAGAIN) {
                return;
            }
            //todo should panic
            cout << "err closed" << endl;
            eventLoop->DeleteReadEvent(fd);
            eventLoop->DeleteWriteEvent(fd);
            close(fd);
            return;
        }else {
            if (n > 0) {
                this->recvLen += n;
                if (this->recvLen == headerLen) {
                    this->parsedHeader = true;
                    this->needRecvLen = this->ReqBodyLen(this->recvMsg, this->recvLen) + headerLen;
                }
            }else {
                //n == 0,recv FIN
                cout << "closed" << endl;
                eventLoop->DeleteReadEvent(fd);
                eventLoop->DeleteWriteEvent(fd);
                close(fd);
                return;
            }
        }
    }
    if (this->parsedHeader) {
        cout << "body start" << endl;
        ssize_t n = read(fd, recvMsg + recvLen, size_t(this->needRecvLen - recvLen));
        if (n < 0) {
            if (errno == EAGAIN) {
                return;
            }
            //todo should panic
            cout << "error closed" << endl;
            eventLoop->DeleteReadEvent(fd);
            eventLoop->DeleteWriteEvent(fd);
            close(fd);
            return;
        }else {
            if (n > 0) {
                this->recvLen += n;
                if (this->recvLen == this->needRecvLen) {
                    this->parsedBody = true;
                }
            }else {
                //n == 0,recv FIN
                cout << "closed" << endl;
                eventLoop->DeleteReadEvent(fd);
                eventLoop->DeleteWriteEvent(fd);
                close(fd);
                return;
            }
        }
    }
    if (this->parsedHeader && this->parsedBody) {
        cout << "write start" << endl;
        this->sendMsg = 0;
        this->Service(this->recvMsg, this->recvLen, &this->sendMsg, &this->needSendLen);
        //todo check return value
        if (this->sendMsg == 0) {
            //todo should panic
            return;
        }
        if (eventLoop->CreateWriteEvent(fd, Message::writeEventHandler, this) < 0) {
            //todo should error
            return;
        }
    }
}
void Message::writeEventHandler(int fd, void* clientData) {
    cout << "do write handler " << fd << endl;
    Message *m = (Message*)clientData;
    m->realWriteEventHandler(fd);
    return;
}

void Message::realWriteEventHandler(int fd) {
    ssize_t n = write(fd, this->sendMsg + this->sendLen, size_t(this->needSendLen - this->sendLen));
    if (n < 0) {
        if (errno == EAGAIN) {
            return;
        }
        //todo should panic
        cout << "error closed" << endl;
        eventLoop->DeleteReadEvent(fd);
        eventLoop->DeleteWriteEvent(fd);
        close(fd);
        return;
    }else {
        if (n > 0) {
            this->sendLen += n;
            if (this->sendLen == this->needSendLen) {
                eventLoop->DeleteWriteEvent(fd);
            }
        }else {
            //todo should panic
            return;
        }
    }
    return;
}
