#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <list>
#include <thread>
#include <atomic>
#include "extends/small_packages/small_packages_base/small_packages_base.h"
namespace small_block_queue{
//not thread_safe
class BlockQueue : small_packages::noncopyable{
public:
    BlockQueue();
    ~BlockQueue();
    void Init();
    void Push(const std::function<void()> &cb);
    void PushWithoutLock(const std::function<void()> &cb);
private:
    void looper();

    std::unique_ptr<std::thread> looperThread_;
    std::list<std::function<void()>> cbs_;
    std::atomic_bool stop_ = {false};
    std::mutex mtx_;
    std::condition_variable cv_;
};
}
