#pragma once
//#include <glog/logging.h>
#include <string>
#include "log_level.h"
#include "log_interface.h"

namespace small_log {
void Init(const std::string &path, const std::string &name);
void EnableTrace();
std::unique_ptr<LogI> Make(Level l, const char* file, uint32_t line);
#define MakeLoggerDEBUG small_log::Make(small_log::DEBUG, __FILE__, __LINE__)
#define MakeLoggerWARNING small_log::Make(small_log::WARNING, __FILE__, __LINE__)
#define MakeLoggerINFO small_log::Make(small_log::INFO, __FILE__, __LINE__)
#define MakeLoggerERROR small_log::Make(small_log::ERROR, __FILE__, __LINE__)
#define MakeLoggerFATAL small_log::Make(small_log::FATAL ,__FILE__, __LINE__)

#define LOG(severity) MakeLogger ## severity->stream()
}
