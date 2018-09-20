#include "small_lock_factory.h"

#include "normal_lock.h"

namespace small_lock{
std::shared_ptr<LockI> MakeLock() {
    return std::make_shared<NormalLock>();
}
std::shared_ptr<RWLockI> MakeRWLock() {
    return std::make_shared<NormalLock>();
}
}//namespace small_lock
