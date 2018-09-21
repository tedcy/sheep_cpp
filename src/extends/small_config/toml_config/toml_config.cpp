#include "toml_config.h"

#include <fstream>

namespace small_config{
void TomlConfig::AddKey(const std::string &key) {
    kvs_.insert({key, TomlConfigValue{}});
}
void TomlConfig::Load(const std::string &name,
        std::string &errMsg) {
    std::ifstream file(name);
    if (!file) {
        errMsg = "open file " + name + "failed";
        return;
    }
    auto pr = toml::parse(file);
    if (!pr.valid()) {
        errMsg = pr.errorReason;
        return;
    }
    auto &data = pr.value;
    for (auto &kv: kvs_) {
        load(data, kv.first);
    }
}
void TomlConfig::load(toml::Value& data, const std::string &key){
    auto &value = kvs_[key];
    auto v = data.find(key);
    if (v) {
        if (v->is<std::string>()) {
            value.status_ = TomlConfigValue::Status::SETSTR;
            value.valueStr_ = v->as<std::string>();
        }else {
            if (v->is<int>()) {
                value.status_ = TomlConfigValue::Status::SETINT;
                value.valueStr_ = v->as<int>();
            }
        }
    }
}
uint64_t TomlConfig::GetInt(const std::string &key, std::string &errMsg) {
    auto iter = kvs_.find(key);
    if (iter == kvs_.end()) {
        errMsg = "key "+ key + " not added";
        return 0;
    }
    if (iter->second.status_ == TomlConfigValue::Status::UNSET) {
        errMsg = "key "+ key + " load failed";
        return 0;
    }
    if (iter->second.status_ == TomlConfigValue::Status::SETSTR) {
        errMsg = "key "+ key + " value is string";
        return 0;
    }
    return iter->second.valueInt_;
}
std::string TomlConfig::GetString(const std::string &key, std::string &errMsg) {
    auto iter = kvs_.find(key);
    if (iter == kvs_.end()) {
        errMsg = "key "+ key + " not added";
        return 0;
    }
    if (iter->second.status_ == TomlConfigValue::Status::UNSET) {
        errMsg = "key "+ key + " load failed";
        return 0;
    }
    if (iter->second.status_ == TomlConfigValue::Status::SETINT) {
        errMsg = "key "+ key + " value is int";
        return 0;
    }
    return iter->second.valueStr_;
}
}
