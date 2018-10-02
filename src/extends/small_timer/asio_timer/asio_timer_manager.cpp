#include "asio_timer_manager.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "small_net.h"

namespace small_timer{
AsioTimerManager& AsioTimerManager::GetInstance() {
    static AsioTimerManager instance_;
    return instance_;
}
AsioTimerManager::AsioTimerManager():
    ios_(small_net::AsioNet::GetInstance().GetIos()){
}
std::shared_ptr<boost::asio::deadline_timer> AsioTimerManager::CreateAsioTimer(uint64_t ms) {
    auto t = std::make_shared<boost::asio::deadline_timer>(ios_, boost::posix_time::milliseconds(ms));
    return t;
}
}//namespace small_timer
