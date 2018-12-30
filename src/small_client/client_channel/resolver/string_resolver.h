#pragma once
#include "resolver_interface.h"
namespace small_client{
namespace resolver{
class StringResolver: public ResolverI{
public:
    StringResolver(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) :
        ips_(ips), port_(port), target_(target){
    }
    void Init(std::string &errMsg) override {
    }
    void SetNotifyWatcherChange(WatcherChangeHandlerT handler) override {
        std::set<std::string> nodes;
        for (auto &ip: ips_) {
            nodes.insert(ip + ":" + std::to_string(port_));
        }
        handler(nodes);
    }
private:
    std::vector<std::string> ips_;
    uint32_t port_;
    std::string target_;
    WatcherChangeHandlerT handler_;
};

class StringResolverFactory: public ResolverFactoryI{
public:
    static StringResolverFactory* GetInstance() {
        static StringResolverFactory instance;
        return &instance;
    }
    std::unique_ptr<ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) override {
        return std::unique_ptr<ResolverI>(new StringResolver(ips, port, target));
    }
private:
    StringResolverFactory() = default;
};
}
}
