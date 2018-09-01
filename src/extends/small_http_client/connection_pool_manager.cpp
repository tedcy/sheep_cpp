#include "connection_pool_manager.h"
#include "connection_pool.h"

namespace small_http_client{

std::shared_ptr<ConnectionPoolManager> ConnectionPoolManager::instance_ = nullptr;

std::shared_ptr<ConnectionPoolManager> ConnectionPoolManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = std::shared_ptr<ConnectionPoolManager>(new ConnectionPoolManager);
    }
    return instance_;
}
ConnectionPoolManager::ConnectionPoolManager():ios_(),connectionPools() {
}
ConnectionPoolManager::~ConnectionPoolManager() {
    work_ = nullptr;
    thread_->join();
}
std::shared_ptr<ConnectionPool> ConnectionPoolManager::get(const std::string &host, const std::string &port) {
    auto poolPtr = connectionPools.find(host + port);
    if (poolPtr == connectionPools.end()) {
        return nullptr;
    }
    return poolPtr->second;
}
void ConnectionPoolManager::work() {
    thread_ = std::make_shared<std::thread>([this](){
                work_ = std::make_shared<boost::asio::io_service::work>(ios_);
                ios_.run();
            });
}
void ConnectionPoolManager::add(const std::string &host,const std::string &port, int size) {
    auto connectionPool = std::shared_ptr<ConnectionPool>(new ConnectionPool(ios_, host, port, size));
    connectionPool->init();
    connectionPools.insert({host + port, connectionPool});
}
void ConnectionPoolManager::remove(const std::string &host, const std::string &port) {
    connectionPools.erase(host + port);
}
}
