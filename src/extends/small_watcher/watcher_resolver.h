#pragma once
#include "small_server.h"
#include "make_watcher.h"
#include "log.h"
namespace small_watcher{
class WatcherResolver: public small_server::resolver::ResolverI{
public:
    WatcherResolver(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target):
        watcher_(small_watcher::MakeWatcher(ips, port)),
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
    std::shared_ptr<small_watcher::WatcherI> watcher_;
};

class WatcherResolverFactory: public small_server::resolver::ResolverFactoryI{
public:
    static WatcherResolverFactory* GetInstance() {
        static WatcherResolverFactory instance;
        small_server::resolver::ResolverManager::GetInstance()->Register("watcher", &instance);
        return &instance;
    }
    std::unique_ptr<small_server::resolver::ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) override {
        return std::unique_ptr<small_server::resolver::ResolverI>(new WatcherResolver(ips, port, target));
    }
private:
    WatcherResolverFactory() = default;
};
}
