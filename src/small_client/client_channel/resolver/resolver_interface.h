#pragma once
#include "extends/small_packages/small_packages.h"
#include <set>
#include <functional>
#include <vector>
namespace small_client{
namespace resolver{
class ResolverI: public small_packages::noncopyable{
public:
using WatcherChangeHandlerT = 
    std::function<void(std::set<std::string> &nodes)>;
    virtual ~ResolverI() = default;
    virtual void Init(std::string &errMsg) = 0;
    virtual void SetNotifyWatcherChange(WatcherChangeHandlerT handler) = 0;
};
class ResolverFactoryI: public small_packages::noncopyable{
public:
    virtual ~ResolverFactoryI() = default;
    virtual std::unique_ptr<ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) = 0;
};
}
}
