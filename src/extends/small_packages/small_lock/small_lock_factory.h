#pragma once
#include "small_lock.h"

#include <memory>

namespace small_lock{
std::shared_ptr<LockI> MakeLock();
std::shared_ptr<RWLockI> MakeRWLock();
class UniqueGuard {
public:
    explicit UniqueGuard(std::shared_ptr<LockI> lock): t_(lock) {
        t_->Lock();
    }
    ~UniqueGuard() {
        t_->UnLock();
    }
private:
    UniqueGuard() = delete;
    UniqueGuard(const UniqueGuard&) = delete;
    UniqueGuard& operator=(const UniqueGuard&) = delete;
private:
    std::shared_ptr<LockI> t_;
};
class UniqueReadGuard {
public:
    explicit UniqueReadGuard(std::shared_ptr<RWLockI> &rWLock): t_(rWLock) {
        t_->RLock();
    }
    ~UniqueReadGuard() {
        t_->RUnLock();
    }
private:
    UniqueReadGuard() = delete;
    UniqueReadGuard(const UniqueReadGuard&) = delete;
    UniqueReadGuard& operator=(const UniqueReadGuard&) = delete;
private:
    std::shared_ptr<RWLockI> &t_;
};
class UniqueWriteGuard {
public:
    explicit UniqueWriteGuard(std::shared_ptr<RWLockI> &rWLock): t_(rWLock) {
        t_->WLock();
    }
    ~UniqueWriteGuard() {
        t_->WUnLock();
    }
private:
    UniqueWriteGuard() = delete;
    UniqueWriteGuard(const UniqueWriteGuard&) = delete;
    UniqueWriteGuard& operator=(const UniqueWriteGuard&) = delete;
private:
    std::shared_ptr<RWLockI> &t_;
};
}//namespace small_lock
