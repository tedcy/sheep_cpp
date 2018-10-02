#include "glog_manager.h"
#include <glog/logging.h>
#include <iostream>

namespace small_log{
bool gDestoryed = false;
GLogFactory& GLogFactory::GetInstance() {
    static GLogFactory manager_;
    return manager_;
}
GLogFactory::~GLogFactory() {
    google::ShutdownGoogleLogging();
    gDestoryed = true;
    std::cout << "~GLogFactory" << std::endl;
}
void GLogFactory::Init(const std::string &path, const std::string &name) {
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
void GLogFactory::EnableTrace() {
    google::InstallFailureSignalHandler();
}
}
