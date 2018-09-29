#include "glog_manager.h"
#include <glog/logging.h>

namespace small_log{
GLogManager& GLogManager::GetInstance() {
    static GLogManager manager_;
    return manager_;
}
LogI& GLogManager::Log(Level l) {
    switch (l) {
        case DEBUG:
            return GLogDebug::GetInstance();
        case WARNING:
            return GLogWarning::GetInstance();
        case INFO:
            return GLogInfo::GetInstance();
        case ERROR:
            return GLogError::GetInstance();
        case FATAL:
            return GLogFatal::GetInstance();
    }
}
void GLogManager::Init(const std::string &path, const std::string &name) {
    if (path == "") {
        FLAGS_log_dir = "./build/log";
    }else {
        FLAGS_log_dir = path;
    }
    FLAGS_max_log_size = 1.8 * 1024;
    FLAGS_stderrthreshold = google::FATAL;
    gName_ = name;
    //fix glog use c str point
    //when name is convert from c str, it's temporary
    //so store it in gName
    google::InitGoogleLogging(gName_.c_str());
}
void GLogManager::EnableTrace() {
    google::InstallFailureSignalHandler();
}
}
