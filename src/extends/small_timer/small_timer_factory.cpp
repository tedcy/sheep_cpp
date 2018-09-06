#include "small_timer_factory.h"
#include "asio_timer.h"
#include "asio_timer_manager.h"

namespace small_timer{
std::shared_ptr<TimerI> MakeTimer() {
    std::shared_ptr<AsioTimer> t = std::make_shared<AsioTimer>();
    return t;
}
void Init() {
    AsioTimerManager::GetInstance();
}
}//namespace small_timer
