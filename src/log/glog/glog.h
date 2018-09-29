#pragma once
#include "log_interface.h"

namespace small_log{
class GLog: public LogI{
    void Log(LogI::Level l) override;
};
}
