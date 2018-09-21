#pragma once

#include <string>
#include <map>

namespace small_config{
class ConfigI{
public:
    virtual ~ConfigI() = default;
    virtual void AddKey(const std::string &key) = 0;
    virtual void Load(const std::string &name, std::string &errMsg) = 0;
    virtual uint64_t GetInt(const std::string &key, std::string &errMsg) = 0;
    virtual std::string GetString(const std::string &key, std::string &errMsg) = 0;
};
}
