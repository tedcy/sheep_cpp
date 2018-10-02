#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace small_timer{
class AsioTimerManager {
public:
    static AsioTimerManager& GetInstance();
    std::shared_ptr<boost::asio::deadline_timer> CreateAsioTimer(uint64_t ms);
    ~AsioTimerManager() = default;
private:
    AsioTimerManager();
    AsioTimerManager(const AsioTimerManager&) = delete;
    AsioTimerManager& operator=(const AsioTimerManager&) = delete;
private:
    boost::asio::io_service &ios_;
};
}
