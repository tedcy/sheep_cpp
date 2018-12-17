#include "client.h"
#include "log.h"
#include "small_server.h"
#include "small_net.h"

int main() {
    std::string errMsg;
    small_server::SheepNetCore::GetInstance()->Init();
    //small_net::AsioNet::GetInstance().Init();
    small_server::RedisCore::GetInstance()->Init(
    errMsg, {"172.16.187.149"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto hold = DoReq(errMsg);
    if (!errMsg.empty()) {
        LOG(INFO) << errMsg;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_server::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
