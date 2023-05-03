#pragma once
#include "log/log.h"
#include "net/net.h"
#include "extends/small_packages/small_packages.h"
#include <thread>
namespace small_client{
//thread unsafe
class Looper: public small_packages::noncopyable{
public:
    static Looper* GetInstance() {
        static Looper instance;
        return &instance;
    }
    ~Looper() {
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
    Looper() = default;
    sheep::net::EventLoop loop_;
    std::unique_ptr<std::thread> thread_;
};
}
