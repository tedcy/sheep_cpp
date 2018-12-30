#pragma once
#include "small_packages.h"
#include "lb_policy/lb_policy.h"
#include "resolver/resolver.h"
namespace small_client{
class Balancer: small_packages::noncopyable{
public:
    Balancer(const std::vector<std::string> &ips, uint32_t port, 
            const std::string &target, 
            const std::string &LbPolicyType = "random",
            const std::string &ResolverType = "string") {
        lbPolicy_ = lb_policy::LbPolicyManager::GetInstance()->GetPolicyFactory(LbPolicyType)->
            Create();
        resolver_ = resolver::ResolverManager::GetInstance()->GetResolverFactory(ResolverType)->
            Create(ips, port, target);
    }
    void Init(std::string &errMsg,
            std::function<void(std::set<std::string> &nodes)> handler) {
        resolver_->Init(errMsg);
        if (!errMsg.empty()) {
            return;
        }
        resolver_->SetNotifyWatcherChange([this, handler](std::set<std::string> &nodes){
            lbPolicy_->UpdateAllWithoutWeight(nodes);
            handler(nodes);
        });
    }
    std::string Get(bool &ok) {
        return lbPolicy_->Get(ok);
    }
private:
    std::unique_ptr<lb_policy::LbPolicyI> lbPolicy_;
    std::unique_ptr<resolver::ResolverI> resolver_;
};
}
