#pragma once

#include "connection_pool.h"

#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <map>
#include <thread>

namespace small_http_client {
class ConnectionPoolManager {
public:
    static std::shared_ptr<ConnectionPoolManager> getInstance();
    std::shared_ptr<ConnectionPool> get(const std::string &host, const std::string &port);
    void work();
    void add(const std::string &host,const std::string &port, int size);
    void remove(const std::string &host,const std::string &port);
    ~ConnectionPoolManager();
private:
    ConnectionPoolManager();
    ConnectionPoolManager(const ConnectionPoolManager&) = delete;
    ConnectionPoolManager& operator = (const ConnectionPoolManager&) = delete;
    boost::asio::io_service ios_;
    std::map<std::string, std::shared_ptr<ConnectionPool>> connectionPools;
    std::shared_ptr<boost::asio::io_service::work> work_ = nullptr;
    std::shared_ptr<std::thread> thread_ = nullptr;
private:
    static std::shared_ptr<ConnectionPoolManager> instance_;
};
}//namespace small_http_client
