#include "etcd_resolver.h"
#include "log.h"
#include "small_packages.h"

namespace small_server{
namespace resolver{
EtcdResolver::EtcdResolver(const std::vector<std::string> &ips, uint32_t port,
        const std::string &target) :
    watcher_(small_watcher::MakeWatcher(ips, port)),
    target_(target) {
}

void EtcdResolver::Init(std::string &errMsg) {
    watcher_->Init(errMsg);
}

void EtcdResolver::SetNotifyWatcherChange(WatcherChangeHandlerT handler) {
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

std::unique_ptr<ResolverI> EtcdResolverFactory::Create(const std::vector<std::string> &ips, uint32_t port,
    const std::string &target) {
    return std::unique_ptr<ResolverI>(new EtcdResolver(ips, port, target));
}
}
}
