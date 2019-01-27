#include "log.h"
#include "small_client.h"

std::shared_ptr<small_client::RedisClient> DoReq(std::string &errMsg,
        small_client::ClientChannel &channel) {
    auto client = std::make_shared<small_client::RedisClient>(channel);
    client->SetTimeoutMs(1);
    client->DoReq("HGETALL test", 
    [](small_client::RedisClient &client, 
        const std::string &errMsg) {
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        bool ok;
        std::vector<std::string> resps;
        std::string errMsg1;
        client.GetResp(errMsg1, ok, resps);
        if (!ok) {
            LOG(WARNING) << "not exist";
            return;
        }
        for (auto &resp: resps) {
            LOG(INFO) << resp;
        }
    });
    return client;
}

int main() {
    std::string errMsg;
    small_client::Looper::GetInstance()->Init();
    small_client::ClientChannel channel(
            small_client::Looper::GetInstance()->GetLoop());
    channel.SetResolverType("string");
    channel.SetMaxSize(10);
    channel.Init(errMsg, {"172.16.187.149"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto hold = DoReq(errMsg, channel);
    if (!errMsg.empty()) {
        LOG(INFO) << errMsg;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_client::Looper::GetInstance()->Shutdown();
}
