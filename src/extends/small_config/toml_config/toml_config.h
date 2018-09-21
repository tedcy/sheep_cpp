#pragma once

#include "small_config.h"
#include <map>

#include "toml/toml.h"

namespace small_config{
class TomlConfig: public ConfigI {
public:
    void AddKey(const std::string &key) override;
    void Load(const std::string &name,
            std::string &errMsg) override;
    uint64_t GetInt(const std::string &key, std::string &errMsg) override;
    std::string GetString(const std::string &key, std::string &errMsg) override;
    struct TomlConfigValue {
        enum Status{
            UNSET,
            SETINT,
            SETSTR,
        };
        Status          status_ = UNSET;
        uint64_t        valueInt_;
        std::string     valueStr_;
    };
private:
    void load(toml::Value& data, const std::string &key);
    std::map<std::string, TomlConfigValue> kvs_;
};
}
