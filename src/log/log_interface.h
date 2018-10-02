#pragma once
#include "log_level.h"
#include <ostream>
#include <memory>

namespace small_log{
class LogI {
public:
    virtual std::ostream& stream() = 0;
    virtual ~LogI() = default;
};
class LogManagerI {
public:
    virtual void Init(const std::string &path, const std::string &name) = 0;
    virtual void EnableTrace() = 0;
    //virtual std::unique_ptr<LogI> Make(Level l, const char *file, uint32_t line) = 0;
    virtual ~LogManagerI() = default;
};
}
