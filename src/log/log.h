#pragma once
//#include <glog/logging.h>
#include <string>
#include "log_level.h"
#include "log_interface.h"

namespace small_log {
void Init(const std::string &path, const std::string &name);
void EnableTrace();
LogFactoryI& GetLogFactory();
#define MakeLoggerDEBUG GetLogFactory().Make(DEBUG, __FILE__, __LINE__)
#define MakeLoggerWARNING GetLogFactory().Make(WARNING, __FILE__, __LINE__)
#define MakeLoggerINFO GetLogFactory().Make(INFO, __FILE__, __LINE__)
#define MakeLoggerERROR GetLogFactory().Make(ERROR, __FILE__, __LINE__)
#define MakeLoggerFATAL GetLogFactory().Make(FATAL ,__FILE__, __LINE__)

#define LOG(severity) MakeLogger ## severity.stream()
}
