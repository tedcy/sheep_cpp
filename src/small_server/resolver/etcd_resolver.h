#pragma once
#include "resolver_interface.h"
#include "small_watcher_factory.h"
namespace small_server{
namespace resolver{
class EtcdResolver: public ResolverI{
public:
    EtcdResolver(const std::vector<std::string> &ips, uint32_t port,
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

class EtcdResolverFactory: public ResolverFactoryI{
public:
    static EtcdResolverFactory* GetInstance() {
        static EtcdResolverFactory instance;
        return &instance;
    }
    std::unique_ptr<ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) override {
        return std::unique_ptr<ResolverI>(new EtcdResolver(ips, port, target));
    }
private:
    EtcdResolverFactory() = default;
};
}
}
