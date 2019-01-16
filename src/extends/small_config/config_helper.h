#pragma once

#include "small_config_interface.h"
#include <memory>
#include <iostream>
#include <string>
#include <map>

namespace small_config{
class ConfigHelper {
public:
    ConfigHelper(std::weak_ptr<ConfigI> config):
        config_(config) {
    }
    void Insert(const std::string &key, std::string &value){
        kvsString_.insert({key, value});
    }
    void Insert(const std::string &key, uint64_t &value){
        kvsInt_.insert({key, value});
    }
    void Parse(std::string &errMsg) {
        auto config = config_.lock();
        if (!config) {
            std::cout << "config destroyed" << std::endl;
        }
        for(auto &kv : kvsString_) {
            config->Get(errMsg, kv.first, kv.second);
            std::cout << kv.first << " : " << kv.second << std::endl;
        }
        for(auto &kv : kvsInt_) {
            config->Get(errMsg, kv.first, kv.second);
            std::cout << kv.first << " : " << kv.second << std::endl;
        }
    }
private:
    std::map<std::string, std::string&> kvsString_;
    std::map<std::string, uint64_t&> kvsInt_;
    std::weak_ptr<ConfigI> config_;
};
}
