#include "log.h"
#include "glog_manager.h"

namespace small_log {
void Init(const std::string &path, const std::string &name) {
    GLogManager::GetInstance().Init(path, name);
}
void EnableTrace() {
    GLogManager::GetInstance().EnableTrace();
}

LogI& LOG(Level l) {
    return GLogManager::GetInstance().Log(l);
}
}
