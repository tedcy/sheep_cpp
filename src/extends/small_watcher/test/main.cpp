#include "small_watcher_factory.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>

#include "small_net.h"

using namespace small_log;

int main() {
    //small_log::Init();
    small_net::AsioNet::GetInstance().Init();
    std::vector<std::string> ips{"172.16.187.149"};
    uint32_t port = 2379;
    auto w = small_watcher::MakeWatcher(ips, port);
    std::string errMsg;
    w->Init(errMsg);
    if (errMsg != "") {
        LOG(ERROR) << errMsg;
    }
    w->CreateEphemeral("/test_watcher", "");
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_net::AsioNet::GetInstance().Shutdown();
}
