#include "log.h"

namespace small_log {
void Init() {
    google::InitGoogleLogging("");
    FLAGS_log_dir = "./build/log";
    FLAGS_max_log_size = 1.8 * 1024;
}
void EnableTrace() {
    google::InstallFailureSignalHandler();
}
}
