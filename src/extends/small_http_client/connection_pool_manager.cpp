#include "connection_pool_manager.h"
#include "connection_pool.h"

#include "small_net.h"

namespace small_http_client{

std::shared_ptr<ConnectionPoolManager> ConnectionPoolManager::instance_ = nullptr;

std::shared_ptr<ConnectionPoolManager> ConnectionPoolManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = std::shared_ptr<ConnectionPoolManager>(new ConnectionPoolManager);
    }
    return instance_;
}
ConnectionPoolManager::ConnectionPoolManager():connectionPools() {
}
std::shared_ptr<ConnectionPool> ConnectionPoolManager::get(const std::string &host, const std::string &port) {
    auto poolPtr = connectionPools.find(host + port);
    if (poolPtr == connectionPools.end()) {
        return nullptr;
    }
    return poolPtr->second;
}
void ConnectionPoolManager::add(const std::string &host,const std::string &port, int size) {
    auto connectionPool = std::shared_ptr<ConnectionPool>(
            new ConnectionPool(small_net::AsioNet::GetInstance()->GetIos(), host, port, size));
    connectionPool->Init();
    connectionPools.insert({host + port, connectionPool});
}
void ConnectionPoolManager::remove(const std::string &host, const std::string &port) {
    connectionPools.erase(host + port);
}
}
