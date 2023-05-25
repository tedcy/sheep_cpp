#pragma once

#include "coroutine_info.h"
#include <memory>
#include <thread>
#include <functional>

class CoroutineScheduler {
public:
    CoroutineScheduler();
    ~CoroutineScheduler();
    
    static CoroutineInfo*& currentCoro() {
        static thread_local CoroutineInfo* currentCoro;
        return currentCoro;
    }

    static CoroutineScheduler*& currentCoroScheduler() {
        static thread_local CoroutineScheduler *currentCoroScheduler;
        return currentCoroScheduler;
    }

    void start() {
        std::thread t([this](){
            run();
        });
        t_ = std::move(t);
    }

    void stop() {
        t_.join();
    }

    bool addCoroutine(const std::function<void()>& func);

    void yield();

    void suspend();

    void resume(CoroutineInfo *coro);

    void sleep(int ms);

private:
    void run();

    class CoroutineSchedulerImp;
    std::unique_ptr<CoroutineSchedulerImp> pimpl_;
    std::thread t_;
};