#include "log.h"

namespace small_log {
std::string gName;
void Init(const std::string &path, const std::string &name) {
//    if (path == "") {
//        FLAGS_log_dir = "./build/log";
//    }else {
//        FLAGS_log_dir = path;
//    }
//    FLAGS_max_log_size = 1.8 * 1024;
//    FLAGS_stderrthreshold = google::FATAL;
//    gName = name;
    //fix glog use c str point
    //when name is convert from c str, it's temporary
    //so store it in gName
//    google::InitGoogleLogging(gName.c_str());
}
void EnableTrace() {
//    google::InstallFailureSignalHandler();
}
}
