#pragma once
#include <mutex>

namespace small_lock {
class LockI{
public:
    virtual void Lock() = 0;
    virtual void UnLock() = 0;
};
class RWLockI {
public:
    virtual void RLock() = 0;
    virtual void RUnLock() = 0;
    virtual void WLock() = 0;
    virtual void WUnLock() = 0;
};
}//namespace small_lock
