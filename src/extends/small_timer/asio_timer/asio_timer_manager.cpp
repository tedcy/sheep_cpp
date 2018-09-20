#include "asio_timer_manager.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "small_net.h"

namespace small_timer{
std::shared_ptr<AsioTimerManager> AsioTimerManager::instance_ = nullptr;
std::shared_ptr<AsioTimerManager> AsioTimerManager::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = std::shared_ptr<AsioTimerManager>(new AsioTimerManager());
    }
    return instance_;
}
AsioTimerManager::AsioTimerManager():
    ios_(small_net::AsioNet::GetInstance()->GetIos()){
}
std::shared_ptr<boost::asio::deadline_timer> AsioTimerManager::CreateAsioTimer(uint64_t ms) {
    auto t = std::make_shared<boost::asio::deadline_timer>(ios_, boost::posix_time::milliseconds(ms));
    return t;
}
}//namespace small_timer
