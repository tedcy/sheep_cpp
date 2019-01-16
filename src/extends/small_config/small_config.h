#pragma once

#include <string>
#include <map>

namespace small_config{
class ConfigI{
public:
    virtual ~ConfigI() = default;
    virtual void Load(const std::string &name, std::string &errMsg) = 0;
    virtual void Get(std::string &errMsg, const std::string &key, std::string &value) = 0;
    virtual void Get(std::string &errMsg, const std::string &key, uint64_t &value) = 0;
};
}
