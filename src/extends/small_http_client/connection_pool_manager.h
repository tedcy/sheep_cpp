#pragma once

#include "connection_pool.h"

#include <string>
#include <map>

namespace small_http_client {
class ConnectionPoolManager {
public:
    static ConnectionPoolManager& GetInstance();
    std::shared_ptr<ConnectionPool> get(const std::string &host, const std::string &port);
    void work();
    void add(const std::string &host,const std::string &port, int size);
    void remove(const std::string &host,const std::string &port);
    ~ConnectionPoolManager();
private:
    ConnectionPoolManager();
    ConnectionPoolManager(const ConnectionPoolManager&) = delete;
    ConnectionPoolManager& operator = (const ConnectionPoolManager&) = delete;
    std::map<std::string, std::shared_ptr<ConnectionPool>> connectionPools;
private:
};
}//namespace small_http_client
