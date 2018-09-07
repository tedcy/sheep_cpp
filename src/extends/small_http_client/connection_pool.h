#pragma once
#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <list>

namespace small_http_client {
class Connection;
//TODO ping test
class ConnectionPool {
public:
    ConnectionPool(boost::asio::io_service &ios, 
            const std::string &host,const std::string &port, int size);
    //TODO clean connections
    ~ConnectionPool() = default;
    void init();
    std::shared_ptr<Connection> get();
    void put(std::shared_ptr<Connection> connection);
    void GetLocalIp(std::string &ip);
private:
    boost::asio::io_service &ios_;
    std::string host_;
    std::string port_;
    int size_ = 0;
    std::list<std::shared_ptr<Connection>> connections_;
};
}//namespace small_http_client
