#include "coroutine_scheduler.h"
#include "coroutine_info.h"
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include "intrusive_list.h"
#include <queue>
#include "util.h"
#include "log.h"

using namespace std;

class CoroutineScheduler::CoroutineSchedulerImp {
public:
    void stop() {
        isRunning_ = false;
    }
    ~CoroutineSchedulerImp() {
        delete [] coros_;
    }
    CoroutineSchedulerImp(int coroNum = 10240) : coros_(new CoroutineInfo[coroNum]){
        IntrusiveListInit(&freeListHead_);
        for (int i = 0;i < coroNum;i++) {
            IntrusiveListInsert(&freeListHead_, &coros_[i]);
        }

        IntrusiveListInit(&suspendListHead_);
        IntrusiveListInit(&resumeListHead_);
        IntrusiveListInit(&yieldListHead_);
    }
    CoroutineSchedulerImp(sheep::net::EventLoop *loop, int coroNum = 10240)
        : CoroutineSchedulerImp(coroNum) {
        loop_ = loop;
    }
    void run() {
        int64_t loopLastRun = UnixTimeMilliSecond();
        while(isRunning_) {
            //为了不让resume唤醒太慢，必须延迟loop_的时间
            //这会导致网络事件的处理延迟，所以tars的网络线程是纯异步的，不使用协程
            if (loop_ && UnixTimeMilliSecond() > loopLastRun + 10) {
                loop_->runOnce();
                loopLastRun = UnixTimeMilliSecond();
            }
            wakeUpSleep();
        
            {
                unique_lock<mutex> lock(mtx_);
                while (!resumeQueue_.empty()) {
                    auto coro = resumeQueue_.front();
                    resumeQueue_.pop();
                    moveToResume(coro);
                }
            }

            auto resumeList = IntrusiveListGetAllNodes(&resumeListHead_);
            for (auto &coro : resumeList) {
                CoroutineInfo::switchTo(coro);
            }
            auto yieldList = IntrusiveListGetAllNodes(&yieldListHead_);
            for (auto &coro : yieldList) {
                CoroutineInfo::switchTo(coro);
            }
        }
    }

    bool addCoroutine(const std::function<void()>& func) {
        unique_lock<mutex> lock(mtx_);
        auto info = IntrusiveListFront(&freeListHead_);
        if (!info) {
            return false;
        }
        info->registerFunc(func, [this, info](){
            IntrusiveListRemove(info);
            IntrusiveListInsert(&freeListHead_, info);
        });;

        moveToYield(info);
        return true;
    }

    void yield() {
        auto cur = CoroutineScheduler::currentCoro();

        moveToYield(cur);

        CoroutineInfo::switchBack();
    }

    void suspend() {
        auto cur = CoroutineScheduler::currentCoro();

        moveToSuspend(cur);

        CoroutineInfo::switchBack();
    }

    // TODO
    // 用coro来唤醒，会造成sleep到期的协程，被resume重复唤醒问题，严重的会因为coro已经不存在导致coredump
    // 应该存在一个suspendID，记录每次休眠的事件ID，如果唤醒的ID和当前ID不一致，就不唤醒
    // 这样就可以避免重复唤醒的问题
    void resume(CoroutineInfo *coro) {
        unique_lock<mutex> lock(mtx_);
        resumeQueue_.push(coro);
    }

    void sleep(int ms) {
        sleepQueue_.push(make_pair(UnixTimeMilliSecond() + ms, CoroutineScheduler::currentCoro()));
        suspend();
    }

    sheep::net::EventLoop& getLoop() {
        return *loop_;
    }
private:
    void wakeUpSleep() {
        vector<CoroutineInfo *> sleepList;
        auto nowMs = UnixTimeMilliSecond();
        while (!sleepQueue_.empty()) {
            auto &top = sleepQueue_.top();
            if (top.first > nowMs) {
                break;
            }
            sleepList.push_back(top.second);
            sleepQueue_.pop();
        }
        for (auto &coro : sleepList) {
            resume(coro);
        }
    }
    void moveToSuspend(CoroutineInfo *info) {
        IntrusiveListRemove(info);
        IntrusiveListInsert(&suspendListHead_, info);
    }
    void moveToResume(CoroutineInfo *info) {
        IntrusiveListRemove(info);
        IntrusiveListInsert(&resumeListHead_, info);
    }
    void moveToYield(CoroutineInfo *info) {
        IntrusiveListRemove(info);
        IntrusiveListInsert(&yieldListHead_, info);
    }
    mutex mtx_;
    CoroutineInfo *coros_;
    CoroutineInfo freeListHead_;
    CoroutineInfo suspendListHead_;
    CoroutineInfo resumeListHead_;
    CoroutineInfo yieldListHead_;
    using sleepInfo = pair<int64_t, CoroutineInfo*>;
    priority_queue<sleepInfo, vector<sleepInfo>, std::greater<sleepInfo>>
        sleepQueue_;
    sheep::net::EventLoop *loop_ = nullptr;
    atomic<bool> isRunning_ = {true};
    queue<CoroutineInfo *> resumeQueue_;
};

CoroutineScheduler::CoroutineScheduler() : pimpl_(new CoroutineSchedulerImp) {}
CoroutineScheduler::CoroutineScheduler(sheep::net::EventLoop *loop)
    : pimpl_(new CoroutineSchedulerImp(loop)) {}
CoroutineScheduler::~CoroutineScheduler() = default;

void CoroutineScheduler::run() {
    CoroutineScheduler::currentCoroScheduler() = this;
    pimpl_->run();
}

void CoroutineScheduler::stop() {
    pimpl_->stop();
    t_.join();
}

bool CoroutineScheduler::addCoroutine(const std::function<void()> &func) {
    return pimpl_->addCoroutine(func);
}

void CoroutineScheduler::yield() {
    pimpl_->yield();
}

void CoroutineScheduler::suspend() {
    pimpl_->suspend();
}

void CoroutineScheduler::resume(CoroutineInfo *coro) {
    pimpl_->resume(coro);
}

void CoroutineScheduler::sleep(int ms) {
    pimpl_->sleep(ms);
}

sheep::net::EventLoop& CoroutineScheduler::getLoop() {
    return pimpl_->getLoop();
}