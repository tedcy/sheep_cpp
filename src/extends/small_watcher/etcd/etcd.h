#pragma once
#include "small_watcher.h"

#include <memory>

//todo 
//1.http req code reuse
//2.watcher
namespace small_watcher{
class Etcd: public WatcherI{
public:
    Etcd(const std::vector<std::string> &ips, uint32_t port);
    ~Etcd() override;
    void Init(std::string &errMsg) override;
    void CreateEphemeral(const std::string &path, 
            const std::string &value) override;
    void GetLocalIp(std::string &ip) override;
    void List(const std::string &path, onListFunc func) override;
    void Watch(const uint64_t afterIndex, 
            const std::string &path, onNotifyFunc func) override;
private:
    void createEphemeral(const std::string &path, 
            const std::string &value, const std::string &errMsg);
    void refresh(const std::string &path, const std::string &value, 
        const std::string &errMsg);
    void nextRefresh(const std::string &path, const std::string &value);
    void nextCreateEphemral(const std::string &path, const std::string &value);
    
    std::vector<std::string> ips_;
    uint32_t port_;
};
}//namespace small_watcher
