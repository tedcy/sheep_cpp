#include "log.h"
#include "glog_manager.h"

#include <memory>

namespace small_log {
LogManagerI& GetLogManager() {
    return GLogManager::GetInstance();
}
void Init(const std::string &path, const std::string &name) {
    GetLogManager().Init(path, name);
}
void EnableTrace() {
    GetLogManager().EnableTrace();
}
std::unique_ptr<LogI> Make(Level l, const char* file, uint32_t line) {
    return std::unique_ptr<LogI>(new GLog(l, file, line));
}
}
