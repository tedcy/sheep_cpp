#pragma once
#include "resolver_interface.h"
#include "etcd_resolver.h"
#include "string_resolver.h"
#include "small_packages.h"
#include <string>
#include <map>
namespace small_server{
namespace resolver{
class ResolverManager: public small_packages::noncopyable{
public:
    static ResolverManager* GetInstance() {
        static ResolverManager instance;
        instance.Register("etcd", EtcdResolverFactory::GetInstance());
        instance.Register("string", StringResolverFactory::GetInstance());
        return &instance;
    }
    void Register(const std::string &type, ResolverFactoryI *factory) {
        ResolverFactorys_.insert({type, factory});
    }
    ResolverFactoryI* GetResolverFactory(const std::string &type) {
        auto iter = ResolverFactorys_.find(type);
        if(iter == ResolverFactorys_.end()) {
            return nullptr;
        }
        return iter->second;
    }
private:
    ResolverManager() = default;
    std::map<std::string, ResolverFactoryI*> ResolverFactorys_;
};
}
}
