#pragma once
#include "log.h"
#include "net.h"
#include "small_packages.h"
#include <thread>
namespace small_client{
//thread unsafe
class SheepNetCore: public small_packages::noncopyable{
public:
    static SheepNetCore* GetInstance() {
        static SheepNetCore redisCore;
        return &redisCore;
    }
    ~SheepNetCore() {
        Shutdown();
    }
    void Init() {
        thread_ = std::unique_ptr<std::thread>(
        new std::thread([this](){
            loop_.Wait();
        }));
    }
    void Shutdown() {
        if (thread_ != nullptr) {
            loop_.Stop();
            thread_->join();
        }
    }
    sheep::net::EventLoop& GetLoop() {
        return loop_;
    }
private:
    SheepNetCore() = default;
    sheep::net::EventLoop loop_;
    std::unique_ptr<std::thread> thread_;
};
}
