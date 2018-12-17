#pragma once
#include "resolver_interface.h"
#include "small_watcher_factory.h"
namespace small_server{
namespace resolver{
class EtcdResolver: public ResolverI{
public:
    EtcdResolver(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target);
    void Init(std::string &errMsg) override;
    void SetNotifyWatcherChange(WatcherChangeHandlerT handler) override;
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
            const std::string &target) override;
private:
    EtcdResolverFactory() = default;
};
}
}
