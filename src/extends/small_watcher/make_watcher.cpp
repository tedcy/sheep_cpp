#include "small_watcher.h"
#include "etcd.h"

namespace small_watcher {
std::shared_ptr<WatcherI> MakeWatcher(const std::vector<std::string> &ips,
        uint32_t port) {
    auto w = std::make_shared<Etcd>(ips, port);
    return w;
}
}//namespace small_watcher
