#include "asio_timer.h"
#include "asio_timer_manager.h"
#include "log.h"

namespace small_timer{
AsioTimer::AsioTimer() {
}
AsioTimer::~AsioTimer() {
}
void AsioTimer::AsyncWait(uint64_t ms, 
            std::function<void(const std::string& errMsg)> onDone) {
    auto t = AsioTimerManager::GetInstance()->CreateAsioTimer(ms);
    t->async_wait([t, onDone](const boost::system::error_code &ec) {
                if (ec) {
                    std::string errMsg = ec.message();
                    onDone(errMsg);
                    return;
                }
                onDone("");
            });
}
}//namespace small_timer
