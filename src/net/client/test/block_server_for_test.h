#pragma once
#include "socket.h"
#include "log.h"
#include "small_packages.h"

#include <ctime>
#include <random>

namespace block_server{
class Server{
public:
    Server() :
        socket_(new sheep::net::Socket()),
        //1024 - 61024
        port_(randomEngine_() % 60000 + 1024){
        LOG(INFO) << "will listen: " << port_;
    }
    int GetPort() {
        return port_;
    }
    void Init() {
        std::string errMsg;
        socket_->Bind(errMsg, "127.0.0.1", port_);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        socket_->SetReuseAddr(errMsg);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        socket_->SetNoDelay(errMsg);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        socket_->Listen(errMsg);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        socket_->SetRcvTimeout(errMsg, 100);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        for (;!stop_;) {
            auto fd = socket_->Accept(errMsg);
            if (!errMsg.empty() || fd < 0) {
                if (!errMsg.empty()) {
                    LOG(WARNING) << errMsg;
                }
                continue;
            }
            acceptedSocket_ = std::unique_ptr<sheep::net::Socket>(new sheep::net::Socket(fd));
            acceptedSocket_->SetNoDelay(errMsg);
            if (!errMsg.empty()) {
                LOG(FATAL) << errMsg;
            }
        }
    }
    void Stop() {
        stop_ = true;
    }
    void Close() {
        acceptedSocket_ = nullptr;
    }
    void Write(char *buf, int len) {
        std::string errMsg;
        acceptedSocket_->Write(errMsg, buf, len);
        if (!errMsg.empty()) {
            return;
        }
    }
    void Read(char *buf, int len) {
        std::string errMsg;
        acceptedSocket_->Read(errMsg, buf, len);
        if (!errMsg.empty()) {
            return;
        }
    }
private:
    std::unique_ptr<sheep::net::Socket> socket_;
    std::unique_ptr<sheep::net::Socket> acceptedSocket_;
    std::default_random_engine randomEngine_ = std::default_random_engine(std::time(0));
    uint32_t port_;
    bool stop_ = false;
};
class ServerThread {
public:
    static ServerThread& GetInstance() {
        static ServerThread instance;
        return instance;
    }
    ServerThread(): 
        thread_([this]() {
            server_.Init();
        }){
    }
    void Stop() {
        server_.Stop();
        thread_.join();
    }
    Server& GetServer() {
        return server_;
    }
private:
    Server server_;
    std::thread thread_;
};
}
