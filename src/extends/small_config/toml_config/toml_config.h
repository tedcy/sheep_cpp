#pragma once

#include "small_config.h"
#include <map>

#include "toml/toml.h"

namespace small_config{
class TomlConfig: public ConfigI {
public:
    void Load(const std::string &name,
            std::string &errMsg) override;
    uint64_t GetInt(const std::string &key, std::string &errMsg) override;
    std::string GetString(const std::string &key, std::string &errMsg) override;
private:
    toml::Value data_; 
};
}
