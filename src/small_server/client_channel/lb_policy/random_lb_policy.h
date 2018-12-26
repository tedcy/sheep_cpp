#pragma once
#include "lb_policy.h"
#include <ctime>
#include <random>
namespace small_server{
namespace lb_policy{
class RandomLbPolicy: public LbPolicyI{
public:
    RandomLbPolicy() = default;
    std::string Get(bool &ok) override {
        if (size_ == 0) {
            ok = false;
            return "";
        }
        ok = true;
        auto index = randomEngine_() % keys_.size();
        return keys_[index];
    }
    void UpdateAllWithoutWeight(std::set<std::string> &keys) override {
        std::vector<std::string> tmpKeys(keys.begin(), keys.end());
        keys_.swap(tmpKeys);
        size_ = keys_.size();
    }
private:
    std::vector<std::string> keys_;
    uint64_t size_;
    std::default_random_engine randomEngine_ = std::default_random_engine(std::time(0));
};

class RandomLbPolicyFactory: public LbPolicyFactoryI {
public:
    static RandomLbPolicyFactory* GetInstance() {
        static RandomLbPolicyFactory instance;
        return &instance;
    }
    std::unique_ptr<LbPolicyI> Create() override {
        return std::unique_ptr<LbPolicyI>{
            new RandomLbPolicy()
        };
    }
private:
    RandomLbPolicyFactory() = default;
};
}
}
