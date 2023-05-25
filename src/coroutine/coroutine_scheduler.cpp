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
#include <iostream>

using namespace std;

class CoroutineScheduler::CoroutineSchedulerImp {
public:
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
    void run() {
        while(1) {
            wakeUpSleep();

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

    void resume(CoroutineInfo *coro) {
        moveToResume(coro);
    }

    void sleep(int ms) {
        sleepQueue_.push(make_pair(UnixTimeMilliSecond() + ms, CoroutineScheduler::currentCoro()));
        suspend();
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
};

CoroutineScheduler::CoroutineScheduler() : pimpl_(new CoroutineSchedulerImp) {
}
CoroutineScheduler::~CoroutineScheduler() = default;

void CoroutineScheduler::run() {
    CoroutineScheduler::currentCoroScheduler() = this;
    pimpl_->run();
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