#pragma once
//#include <glog/logging.h>
#include <string>
#include "log_level.h"
#include "log_interface.h"

namespace small_log {
void Init(const std::string &path, const std::string &name);
void EnableTrace();
std::unique_ptr<LogI> Make(Level l, const char* file, uint32_t line);
#define MakeLoggerDEBUG Make(DEBUG, __FILE__, __LINE__)
#define MakeLoggerWARNING Make(WARNING, __FILE__, __LINE__)
#define MakeLoggerINFO Make(INFO, __FILE__, __LINE__)
#define MakeLoggerERROR Make(ERROR, __FILE__, __LINE__)
#define MakeLoggerFATAL Make(FATAL ,__FILE__, __LINE__)

#define LOG(severity) MakeLogger ## severity->stream()
}
