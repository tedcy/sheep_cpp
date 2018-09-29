#pragma once
#include <string>

namespace small_log{
class LogI {
public:
    virtual LogI& operator<<(int64_t value) = 0;
    virtual LogI& operator<<(const std::string &value) = 0;
};
}
