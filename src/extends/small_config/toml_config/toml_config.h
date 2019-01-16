#pragma once

#include "small_config_interface.h"
#include <map>

#include "toml/toml.h"

namespace small_config{
class TomlConfig: public ConfigI {
public:
    void Load(const std::string &name,
            std::string &errMsg) override;
    void Get(std::string &errMsg, const std::string &key, std::string &value) override;
    void Get(std::string &errMsg, const std::string &key, uint64_t &value) override;
private:
    toml::Value data_; 
};
}
