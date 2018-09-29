#pragma once
//#include <glog/logging.h>
#include <string>
#include "log_interface.h"

//TODO be interface
namespace small_log {
enum Level {
    DEBUG,
    WARNING,
    INFO,
    ERROR,
    FATAL,
};
void Init(const std::string &path, const std::string &name);
void EnableTrace();
LogI& LOG(Level l);
}
