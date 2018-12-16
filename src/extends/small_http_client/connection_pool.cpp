#include "connection_pool.h"
#include "connection.h"
#include "log.h"

namespace small_http_client {
ConnectionPool::ConnectionPool(boost::asio::io_service &ios, 
        const std::string &host,const std::string &port, int size) :
    ios_(ios), host_(host), port_(port), size_(size), connections_(),
    lock_(small_lock::MakeLock()) {
}
ConnectionPool::~ConnectionPool() {
    //LOG(INFO) << "~ConnectionPool";
}

void ConnectionPool::Init() {
    for (int i = 0;i < size_;i++) {
        InitOne();
    }
}

void ConnectionPool::InitOne() {
    auto connection = std::shared_ptr<Connection>(new Connection(ios_, host_, port_));
    auto onConnect = [this,c = std::weak_ptr<Connection>(connection)]
    (const std::string &errMsg) {
        auto connection = c.lock();
        if (!connection) {
            return;
        }
        if (errMsg != "") {
            LOG(ERROR) << host_ << ":" << port_ << " " << errMsg;
            return;
        }
        connection->StartRead();
        //LOG(INFO) << host_ << ":" << port_ << " connected";
        put(connection);
    };
    auto onClose = [this, c = std::weak_ptr<Connection>(connection)]
    (const std::string &resp,
    const std::shared_ptr<std::map<std::string,std::string>> respHeaders,
    const std::string &errMsg){
        auto connection = c.lock();
        if (!connection) {
            return;
        }
        if (errMsg != "") {
            //LOG(ERROR) << host_ << ":" << port_ << " " << errMsg;
            {
                small_lock::UniqueGuard uniqueLock(lock_);
                connections_.remove(connection);
            }
            InitOne();
        }
    };
    //fix: init one is other thread, so set close first before init
    //otherwise when on close first then set
    //lead to core
    connection->SetClose(onClose);
    connection->init(onConnect);
}

void ConnectionPool::GetLocalIp(std::string &ip) {
    auto connection = get();
    if(connection != nullptr) {
        connection->GetLocalIp(ip);
        put(connection);
    }
}

std::shared_ptr<Connection> ConnectionPool::get() {
    small_lock::UniqueGuard uniqueLock(lock_);
    if (connections_.empty()) {
        return nullptr;
    }
    LOG(INFO) << connections_.size();
    auto connection = connections_.front();
    connections_.pop_front();
    return connection;
}

void ConnectionPool::put(std::shared_ptr<Connection> connection) {
    if (!connection->Available()) {
        return;
    }
    if (connection == nullptr) {
        return;
    }
    //LOG(INFO) << "connection put";
    small_lock::UniqueGuard uniqueLock(lock_);
    connections_.push_back(connection);
}
}
