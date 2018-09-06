#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <thread>

namespace small_timer{
class AsioTimerManager {
public:
    static std::shared_ptr<AsioTimerManager> GetInstance();
    std::shared_ptr<boost::asio::deadline_timer> CreateAsioTimer(uint64_t ms);
    ~AsioTimerManager();
private:
    AsioTimerManager();
    AsioTimerManager(const AsioTimerManager&) = delete;
    AsioTimerManager& operator=(const AsioTimerManager&) = delete;
    void Work();
    boost::asio::io_service ios_;
    std::shared_ptr<boost::asio::io_service::work> work_ = nullptr;
    std::shared_ptr<std::thread> thread_ = nullptr;
private:
    static std::shared_ptr<AsioTimerManager> instance_;
};
}
