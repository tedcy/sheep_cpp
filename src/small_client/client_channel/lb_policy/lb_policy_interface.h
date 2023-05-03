#pragma once
#include "extends/small_packages/small_packages.h"
#include <string>
#include <set>
#include <memory>
namespace small_client{
namespace lb_policy{
class LbPolicyI: public small_packages::noncopyable {
public:
    virtual ~LbPolicyI() = default;
    //for balancer
    virtual std::string Get(bool &ok) = 0;
    //for resolver
    virtual void UpdateAllWithoutWeight(std::set<std::string> &keys) = 0;

    //TODO
    //for weighter
    //UpdateAll(std::map<std::string, std::string> &map)
    //for breaker
    //Enable(const std::string &key)
    //Disable(const std::string &key)
};

class LbPolicyFactoryI: public small_packages::noncopyable {
public:
    virtual ~LbPolicyFactoryI() = default;
    virtual std::unique_ptr<LbPolicyI> Create() = 0;
};
}
}
