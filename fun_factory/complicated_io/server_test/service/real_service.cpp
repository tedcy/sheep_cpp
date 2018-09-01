#include "real_service.h"
#include "../../server/buffer/buffer.h"
#include "../../common/common.h"
#include <memory>
#include <iostream>
using namespace std;

/*
Event() {
if !init
ServiceBefore() read/write
if clean || init 
Service()
}
*/
void RealService::Handler() {
    char *data;
    switch (stage_) {
        case RealServiceStage::Init:
            cout << "Init" << endl;
            buffer_ = std::make_shared<BufferWrapper>(1);
            this->AddServerEvent(Option::Read, buffer_);
            stage_ = RealServiceStage::ParseCmd;
            return;
        case RealServiceStage::ParseCmd:
            cout << "ParseCmd" << endl;
            data = new char[1];
            buffer_->Buffer::Read(data, 0, 1);
            cout << data << endl;
            delete []data;
            buffer_ = std::make_shared<BufferWrapper>(5);
            this->AddServerEvent(Option::Read, buffer_);
            stage_ = RealServiceStage::RecvReq;
            return;
        case RealServiceStage::RecvReq:
            cout << "RecvReq" << endl;
            data = new char[5];
            buffer_->Buffer::Read(data, 0, 5);
            cout << data << endl;
            delete []data;
            data = new char[11];
            buffer_ = std::make_shared<BufferWrapper>(data, 11);
            delete []data;
            this->AddServerEvent(Option::Write, buffer_);
            stage_ = RealServiceStage::SendResp;
            return;
        case RealServiceStage::SendResp:
            cout << "SendResp" << endl;
            this->NextService();
            return;
    }
}

void RealService::EndHandler() {
    cout << "End" << endl;
}

/*
void RealService::Service() {
    if stage == "init" {
        buffer.SetSize(1);
        ServeEvent(Read, buffer);
    }
    if stage == "parse_cmd" {
        parse(cmd);
    }
    if cmd == "proxy" {
        if stage == "recv_file" {
            buffer.SetSize(100);
            //事件结束以后自动注销相关事件
            auto event = AddServerEvent(Read, buffer);
            if (event.fail()) {
                return;
            }
        }
        if stage == "write_file" {
            buffer.write("", n);
            FileEvent(Write, buffer, "filename");
            buffer.write("", n);
            FileEvent(Write, buffer, "filename");
            buffer.write("", n);
            FileEvent(Write, buffer, "filename");
            buffer.write("", n);
            FileEvent(Write, buffer, "filename");
        }
        if stage == "send_file" {
            buffer.write("", n);
            ClientEvent(Write, buffer, "ip:port");
        }
        if stage == "recv_resp" {
            buffer.SetSize(10);
            ClientEvent(Read, buffer, "ip:port")
        }
        if stage == "send_resp" {
            buffer.write("", n);
            ServerEvent(Write, buffer);
        }
    }
    if cmd == "server" {
        if stage == "recv_file" {
            buffer.SetSize(100);
            ServerEvent(Read, buffer)
        }
        if stage == "server_write_file" {
            buffer.write("", n);
            FileEvent(Write, buffer, "filename-1");
        }
        if stage == "server_send_resp" {
            buffer.write("", n);
            ServerEvent(Write, buffer);
        }
    }
    return;
}
*/
