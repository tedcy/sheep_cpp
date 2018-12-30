#include "client.h"
#include "log.h"
#include "small_server.h"

int main() {
    std::string errMsg;
    small_server::SheepNetCore::GetInstance()->Init();
    small_server::RedisCore redisCore;
    redisCore.Init(errMsg, {"172.16.187.149"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto hold = DoReq(errMsg, redisCore);
    if (!errMsg.empty()) {
        LOG(INFO) << errMsg;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_server::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
