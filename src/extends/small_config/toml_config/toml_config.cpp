#include "toml_config.h"

#include <fstream>

namespace small_config{
void TomlConfig::Load(const std::string &name,
        std::string &errMsg) {
    std::ifstream file(name);
    if (!file) {
        errMsg = "open file " + name + " failed";
        return;
    }
    auto pr = toml::parse(file);
    if (!pr.valid()) {
        errMsg = pr.errorReason;
        return;
    }
    data_ = pr.value;
}
void TomlConfig::Get(std::string &errMsg, const std::string &key, uint64_t &value) {
    auto v = data_.find(key);
    if (v) {
        if (v->is<int>()) {
            value = v->as<int>();
            return;
        }else {
            errMsg = "key "+ key + " value is string";
        }
    }else {
        errMsg = "key "+ key + " load failed";
    }
    return;
}
void TomlConfig::Get(std::string &errMsg, const std::string &key, std::string &value) {
    auto v = data_.find(key);
    if (v) {
        if (v->is<std::string>()) {
            value = v->as<std::string>();
            return;
        }else {
            errMsg = "key "+ key + " value is int";
        }
    }else {
        errMsg = "key "+ key + " load failed";
    }
    return;
}
}
