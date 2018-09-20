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
    timer_ = AsioTimerManager::GetInstance()->CreateAsioTimer(ms);
    timer_->async_wait([onDone](const boost::system::error_code &ec) {
                if (ec) {
                    std::string errMsg = ec.message();
                    onDone(errMsg);
                    return;
                }
                onDone("");
            });
}
void AsioTimer::Cancel() {
    if(timer_) {
        timer_->cancel();
    }
}
}//namespace small_timer
