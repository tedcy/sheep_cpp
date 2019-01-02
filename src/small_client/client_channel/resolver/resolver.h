#pragma once
#include "resolver_interface.h"
#include "string_resolver.h"
#include "dns_resolver.h"
#include "small_packages.h"
#include <string>
#include <map>
#include "log.h"
namespace small_client{
namespace resolver{
class ResolverManager: public small_packages::noncopyable{
public:
    static ResolverManager* GetInstance() {
        static ResolverManager instance;
        instance.Register("string", StringResolverFactory::GetInstance());
        instance.Register("dns", DnsResolverFactory::GetInstance());
        return &instance;
    }
    void Register(const std::string &type, ResolverFactoryI *factory) {
        ResolverFactorys_.insert({type, factory});
    }
    ResolverFactoryI* GetResolverFactory(const std::string &type) {
        auto iter = ResolverFactorys_.find(type);
        if(iter == ResolverFactorys_.end()) {
            LOG(FATAL) << "type " << type << " do not exist";
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
