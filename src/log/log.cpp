#include "log.h"
#include "glog_manager.h"

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
}
