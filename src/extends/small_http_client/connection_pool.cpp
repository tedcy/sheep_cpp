#include "connection_pool.h"
#include "connection.h"
#include "log.h"

namespace small_http_client {
ConnectionPool::ConnectionPool(boost::asio::io_service &ios, 
        const std::string &host,const std::string &port, int size) :
    ios_(ios), host_(host), port_(port), size_(size), connections_() {
}

void ConnectionPool::init() {
    for (int i = 0;i < size_;i++) {
        std::function<void(const std::string&)> onConnect;
        auto connection = std::shared_ptr<Connection>(new Connection(ios_, host_, port_));
        onConnect = [this,connection](const std::string &errMsg) {
            if (errMsg != "") {
                LOG(ERROR) << host_ << ":" << port_ << " " << errMsg;
            }
            connections_.push_back(connection);
        };
        connection->init(onConnect);
    }
}

void ConnectionPool::GetLocalIp(std::string &ip) {
    auto connection = get();
    if(connection != nullptr) {
        connection->GetLocalIp(ip);
        put(connection);
    }
}

std::shared_ptr<Connection> ConnectionPool::get() {
    if (connections_.empty()) {
        return nullptr;
    }
    auto connection = connections_.front();
    connections_.pop_front();
    return connection;
}

void ConnectionPool::put(std::shared_ptr<Connection> connection) {
    if (connection == nullptr) {
        return;
    }
    connections_.push_back(connection);
}

//std::shared_ptr<Connection> Get() {
//}
}
