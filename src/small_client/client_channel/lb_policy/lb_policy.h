#pragma once
#include "lb_policy_interface.h"
#include "random_lb_policy.h"
#include "small_packages.h"
#include <string>
#include <map>
namespace small_client{
namespace lb_policy{
class LbPolicyManager: public small_packages::noncopyable{
public:
    static LbPolicyManager* GetInstance() {
        static LbPolicyManager instance;
        instance.Register("random", RandomLbPolicyFactory::GetInstance());
        return &instance;
    }
    void Register(const std::string &type, LbPolicyFactoryI *factory) {
        lBPolicyFactorys_.insert({type, factory});
    }
    LbPolicyFactoryI* GetPolicyFactory(const std::string &type) {
        auto iter = lBPolicyFactorys_.find(type);
        if(iter == lBPolicyFactorys_.end()) {
            return nullptr;
        }
        return iter->second;
    }
private:
    LbPolicyManager() = default;
    std::map<std::string, LbPolicyFactoryI*> lBPolicyFactorys_;
};
}
}
