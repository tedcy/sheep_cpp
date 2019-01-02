#include "small_block_queue.h"

namespace small_block_queue {
BlockQueue::BlockQueue() {
}
BlockQueue::~BlockQueue() {
    stop_ = true;
    if (looperThread_ != nullptr) {
        looperThread_->join();
    }
}
void BlockQueue::Init() {
    if (looperThread_ != nullptr) {
        return;
    }
    looperThread_ = std::unique_ptr<std::thread>(
        new std::thread([this](){
            looper();
        })
    );
}
void BlockQueue::Push(const std::function<void()> &cb) {
    std::unique_lock<std::mutex> lock(mtx_);
    PushWithoutLock(cb);
}
void BlockQueue::PushWithoutLock(const std::function<void()> &cb) {
    cbs_.push_back(cb);
    cv_.notify_one();
}
void BlockQueue::looper() {
    while(!stop_) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() {
            return !cbs_.empty();
        });
        for (auto &cb: cbs_) {
            cb();
        }
    }
}
}
