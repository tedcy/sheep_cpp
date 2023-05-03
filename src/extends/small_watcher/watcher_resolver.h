#pragma once
#include "small_client/small_client.h"
#include "make_watcher.h"
#include "log/log.h"
namespace small_watcher{
class WatcherResolver: public small_client::resolver::ResolverI{
public:
    WatcherResolver(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target):
        watcher_(MakeWatcher(ips, port)),
        target_(target) {
    }
    void Init(std::string &errMsg) override {
        watcher_->Init(errMsg);
    }
    void SetNotifyWatcherChange(WatcherChangeHandlerT handler) override {
        handler_ = handler;
        watcher_->ListWatch(target_, [this, handler](const std::string &argErrMsg, bool &stop,
            std::shared_ptr<std::vector<std::string>> keys){
            std::set<std::string> nodes;
            for (auto &key : *keys) {
                auto realKey = small_strings::TrimLeft(key, target_ + "/"); 
                nodes.insert(realKey);
            }
            handler(nodes);
        });
    }
private:
    std::string target_;
    WatcherChangeHandlerT handler_;
    std::shared_ptr<WatcherI> watcher_;
};

class WatcherResolverFactory: public small_client::resolver::ResolverFactoryI{
public:
    static WatcherResolverFactory* GetInstance() {
        static WatcherResolverFactory instance;
        return &instance;
    }
    void Init() {
        small_client::resolver::ResolverManager::GetInstance()->
            Register("watcher", WatcherResolverFactory::GetInstance());
    }
    std::unique_ptr<small_client::resolver::ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) override {
        return std::unique_ptr<small_client::resolver::ResolverI>(new WatcherResolver(ips, port, target));
    }
private:
    WatcherResolverFactory() = default;
};
}
