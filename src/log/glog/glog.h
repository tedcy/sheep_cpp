#pragma once
#include "log.h"
#include "log_interface.h"
#include <string>
#include <glog/logging.h>
#include <memory>

namespace small_log{
class GLog: public LogI{
public:
    GLog(Level l, const char *file, uint32_t line, bool destoryed);
    std::ostream& stream() override;
private:
    std::unique_ptr<google::LogMessage> message_;
    std::unique_ptr<std::ostream> stream_;
};
}
