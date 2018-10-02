#pragma once

#include <thread>
#include <memory>
#include <boost/asio.hpp>

namespace small_net{
class AsioNet{
public:
    static AsioNet& GetInstance();
    void Init();
    boost::asio::io_service& GetIos();
    ~AsioNet();
    void Shutdown();
private:
    AsioNet() = default;
    AsioNet(const AsioNet&) = delete;
    AsioNet& operator=(const AsioNet&) = delete;

    boost::asio::io_service ios_;
    std::shared_ptr<boost::asio::io_service::work> work_ = nullptr;
    std::shared_ptr<std::thread> thread_ = nullptr;
};
}
