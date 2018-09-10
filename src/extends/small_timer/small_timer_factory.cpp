#include "small_timer_factory.h"
#include "asio_timer.h"
#include "asio_timer_manager.h"
#include <chrono>

namespace small_timer{
std::shared_ptr<TimerI> MakeTimer() {
    std::shared_ptr<AsioTimer> t = std::make_shared<AsioTimer>();
    return t;
}
void Init() {
    AsioTimerManager::GetInstance();
}
uint64_t UnixTimeSecond() {
    std::chrono::time_point<std::chrono::system_clock,std::chrono::seconds> tp = 
        std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return uint64_t(timestamp);
}
}//namespace small_timer
