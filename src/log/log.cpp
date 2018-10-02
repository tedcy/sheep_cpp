#include "log.h"
#include "glog_manager.h"

#include <memory>

namespace small_log {
void Init(const std::string &path, const std::string &name) {
    GetLogFactory().Init(path, name);
}
void EnableTrace() {
    GetLogFactory().EnableTrace();
}
LogFactoryI& GetLogFactory() {
    return GLogFactory::GetInstance();
}
std::unique_ptr<LogI> Make(Level l, const char* file, uint32_t line) {
    return std::unique_ptr<LogI>(new GLog(l, file, line));
}
}
