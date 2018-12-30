#include "log.h"
#include "small_client.h"

std::shared_ptr<small_client::RedisClient> DoReq(std::string &errMsg,
        small_client::SheepNetClientCore &redisCore) {
    auto client = std::make_shared<small_client::RedisClient>(redisCore);
    client->DoReq("GET A", 
    [](small_client::RedisClient &client, 
        const std::string &errMsg) {
        LOG(INFO) << "DoReq Done";
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        bool ok;
        auto resp = client.GetResp(ok);
        if (!ok) {
            LOG(WARNING) << "not exist";
            return;
        }
        LOG(INFO) << resp;
    });
    return client;
}

int main() {
    std::string errMsg;
    small_client::SheepNetCore::GetInstance()->Init();
    small_client::SheepNetClientCore core(
            small_client::SheepNetCore::GetInstance()->GetLoop());
    core.SetResolverType("string");
    core.SetMaxSize(10);
    core.Init(errMsg, {"172.16.187.149"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto hold = DoReq(errMsg, core);
    if (!errMsg.empty()) {
        LOG(INFO) << errMsg;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_client::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
