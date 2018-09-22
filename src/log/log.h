#pragma once
#include <glog/logging.h>
#include <string>

//TODO be interface
namespace small_log {
void Init(const std::string &path, const std::string &name);
void EnableTrace();
}
