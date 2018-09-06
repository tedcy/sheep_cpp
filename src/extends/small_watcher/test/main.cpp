#include "small_watcher_factory.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>

int main() {
    //small_log::Init();
    std::vector<std::string> ips{"172.16.176.38"};
    uint32_t port = 2379;
    auto w = small_watcher::MakeWatcher(ips, port);
    std::string errMsg;
    w->Init(errMsg);
    if (errMsg != "") {
        LOG(ERROR) << errMsg;
    }
    w->CreateEphemeral("test_watcher", "");
    std::this_thread::sleep_for(std::chrono::seconds(100));
}
