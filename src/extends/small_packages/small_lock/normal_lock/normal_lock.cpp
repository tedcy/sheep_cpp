#include "normal_lock.h"

namespace small_lock {
void NormalLock::Lock() {
    lock_.lock();
}
void NormalLock::UnLock() {
    lock_.unlock();
}
void NormalLock::RLock() {
    lock_.lock();
}
void NormalLock::RUnLock() {
    lock_.unlock();
}
void NormalLock::WLock() {
    lock_.lock();
}
void NormalLock::WUnLock() {
    lock_.unlock();
}
}//namespace small_lock
