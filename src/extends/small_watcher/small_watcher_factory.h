#pragma once
#include "small_watcher.h"

#include <memory>

namespace small_watcher {
std::shared_ptr<WatcherI> MakeWatcher(const std::vector<std::string> &ips,
        uint32_t port);
}//namespace small_watcher
