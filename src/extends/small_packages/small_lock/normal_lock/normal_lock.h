#pragma once
#include <mutex>
#include "small_lock.h"

namespace small_lock{
class NormalLock: public LockI, public RWLockI{
public:
    void Lock() override;
    void UnLock() override;
    void RLock() override;
    void RUnLock() override;
    void WLock() override;
    void WUnLock() override;
private:
    std::mutex lock_;
};
}//small_lock
