#include "connection_pool_manager.h"
#include "connection_pool.h"

#include "small_net.h"
#include "log.h"

using namespace small_log;

namespace small_http_client{

ConnectionPoolManager& ConnectionPoolManager::GetInstance() {
    static ConnectionPoolManager instance_;
    return instance_;
}
ConnectionPoolManager::ConnectionPoolManager():connectionPools() {
}
ConnectionPoolManager::~ConnectionPoolManager() {
    //LOG(INFO) << "~ConnectionPoolManager";
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
            new ConnectionPool(small_net::AsioNet::GetInstance().GetIos(), host, port, size));
    connectionPool->Init();
    connectionPools.insert({host + port, connectionPool});
}
void ConnectionPoolManager::remove(const std::string &host, const std::string &port) {
    connectionPools.erase(host + port);
}
}
