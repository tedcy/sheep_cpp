#pragma once
#include "small_watcher_interface.h"
#include "small_server.h"
#include "net.h"

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
    void WatchOnce(const uint64_t afterIndex, 
            const std::string &path, onNotifyFunc func) override;
    void ListWatch(const std::string &path, 
            const onListWatchFunc &func) override;
private:
    void createEphemeral(const std::string &path, 
            const std::string &value, const std::string &errMsg);
    void refresh(const std::string &path, const std::string &value, 
        const std::string &errMsg);
    void nextRefresh(const std::string &path, const std::string &value);
    void nextCreateEphemral(const std::string &path, const std::string &value);

    void watch(const uint64_t afterIndex, 
        const std::string &path,
        const onListWatchFunc &func,
        const std::function<void()> &optFunc);
    void listWatch(const std::string &path, const onListWatchFunc &func);
    
    std::vector<std::string> ips_;
    uint32_t port_;
    std::shared_ptr<small_server::SheepNetClientCore> core_;
    std::set<std::shared_ptr<small_server::HttpClientBackUp>> httpClients_;
    std::set<std::shared_ptr<sheep::net::Timer>> timers_;
};
}//namespace small_watcher
