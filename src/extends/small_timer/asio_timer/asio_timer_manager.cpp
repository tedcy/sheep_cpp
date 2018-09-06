#include "asio_timer_manager.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace small_timer{
std::shared_ptr<AsioTimerManager> AsioTimerManager::instance_ = nullptr;
std::shared_ptr<AsioTimerManager> AsioTimerManager::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = std::shared_ptr<AsioTimerManager>(new AsioTimerManager());
        instance_->Work();
    }
    return instance_;
}
AsioTimerManager::AsioTimerManager():ios_() {
}
AsioTimerManager::~AsioTimerManager() {
    work_ = nullptr;
    thread_->join();
}
void AsioTimerManager::Work() {
    thread_ = std::make_shared<std::thread>([this](){
                work_ = std::make_shared<boost::asio::io_service::work>(ios_);
                ios_.run();
            });
}
std::shared_ptr<boost::asio::deadline_timer> AsioTimerManager::CreateAsioTimer(uint64_t ms) {
    auto t = std::make_shared<boost::asio::deadline_timer>(ios_, boost::posix_time::milliseconds(ms));
    return t;
}
}//namespace small_timer
