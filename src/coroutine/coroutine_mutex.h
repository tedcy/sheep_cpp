#pragma once

#include "coroutine_scheduler.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include "log.h"

struct CoroutineWaiter {
    CoroutineScheduler* sched_ = CoroutineScheduler::currentCoroScheduler();
    CoroutineInfo *info_ = CoroutineScheduler::currentCoro();
    bool isCanceled = false;

    void wakeup() {
        if (!sched_) return;
        sched_->resume(info_);
    }

    //TODO 实现返回值isTimeout
    void wait(std::unique_lock<std::mutex>& lock, int milliseconds) {
        if (!sched_) {
            throw std::runtime_error(
                "CoroutineCondition::wait() must be called in coroutine");
        }
        if (milliseconds == -1) {
            sched_->suspend();
        } else {
            sched_->sleep(milliseconds);
        }
        lock.lock();
        isCanceled = true;
    }
};

class CoroutineConditionVariable {
public:
    void wait(std::unique_lock<std::mutex>& lock, int milliseconds = -1) {
        CoroutineWaiter waiter;

        waiters_.push(waiter);
        lock.unlock();

        waiter.wait(lock, milliseconds);
    }
    int notify_one() { return notify_nolock(false); }
    int notify_all() { return notify_nolock(true); }

private:
    int notify_nolock(bool isAll) {
        int notifyCount = 0;
        while (!waiters_.empty()) {
            auto& front = waiters_.front();
            if (front.isCanceled) {
                waiters_.pop();
                continue;
            }
            front.wakeup();
            waiters_.pop();
            notifyCount++;
            if (!isAll) {
                break;
            }
        }
        return notifyCount;
    }
    std::queue<CoroutineWaiter> waiters_;
};

class CoroutineMutex {
    std::mutex mtx_;
    std::atomic<bool> isLock_ = {false};
    std::queue<CoroutineWaiter> waiters_;
    bool setLock(bool newV) {
        bool expected = !newV;
        return isLock_.compare_exchange_strong(expected, newV);
    }
    int notify_nolock(bool isAll) {
        int notifyCount = 0;
        while (!waiters_.empty()) {
            auto& front = waiters_.front();
            if (front.isCanceled) {
                waiters_.pop();
                continue;
            }
            front.wakeup();
            waiters_.pop();
            notifyCount++;
            if (!isAll) {
                break;
            }
        }
        return notifyCount;
    }
public:
    void lock() {
        if (!setLock(true)) {
            CoroutineWaiter waiter;
            std::unique_lock<std::mutex> lock(mtx_);
            //再次尝试，也是为了把isLock_和waitings_放在同一个临界区内
            if (setLock(true)) {
                return;
            }
            waiters_.push(waiter);
            lock.unlock();
            waiter.wait(lock, -1);
        }
    }
    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);
        //把isLock_和waitings_放在同一个临界区内
        //如果唤醒成功，就传递isLock_的所有权到下一个协程
        //否则，就解开isLock_所有权
        if (notify_nolock(false) == 0) {
            setLock(false);
        }
    }
};

class ConditionVariable {
    CoroutineConditionVariable coCv_;
    std::condition_variable cv_;

public:
    void wait(std::unique_lock<std::mutex>& lock, int milliseconds = -1) {
        if (CoroutineScheduler::currentCoroScheduler()) {
            coCv_.wait(lock, milliseconds);
            return;
        }
        if (milliseconds != -1) {
            cv_.wait_for(lock, std::chrono::milliseconds(milliseconds));
            return;
        }
        cv_.wait(lock);
    }
    void notify_one() {
        if (coCv_.notify_one() == 0) {
            cv_.notify_one();
        }
    }
    void notify_all() {
        coCv_.notify_all();
        cv_.notify_all();
    }
};

class Mutex {
    std::mutex mtx_;
    ConditionVariable cond_;
    bool isLock = false;
    int waitings_ = 0;

public:
    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);  // 加锁保护临界区
        while (isLock) {
            waitings_++;
            cond_.wait(lock);  // 等待 isLock 状态变化
            waitings_--;
        }
        isLock = true;
    }

    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);  // 加锁保护临界区
        isLock = false;
        if (waitings_ > 0) {
            cond_.notify_one();  // 唤醒一个等待者
        }
    }
};