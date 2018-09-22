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
uint64_t TomlConfig::GetInt(const std::string &key, std::string &errMsg) {
    auto v = data_.find(key);
    if (v) {
        if (v->is<int>()) {
            return v->as<int>();
        }else {
            errMsg = "key "+ key + " value is string";
        }
    }else {
        errMsg = "key "+ key + " load failed";
    }
    return 0;
}
std::string TomlConfig::GetString(const std::string &key, std::string &errMsg) {
    auto v = data_.find(key);
    if (v) {
        if (v->is<std::string>()) {
            return v->as<std::string>();
        }else {
            errMsg = "key "+ key + " value is int";
        }
    }else {
        errMsg = "key "+ key + " load failed";
    }
    return "";
}
}
