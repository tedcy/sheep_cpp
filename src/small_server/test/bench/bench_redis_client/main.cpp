#include "client.h"
#include "log.h"
#include "small_server.h"
#include "small_net.h"

void DoReq(std::string &errMsg,
        std::set<std::shared_ptr<small_server::RedisClient>> &clients) {
    auto clientPtr = std::make_shared<small_server::RedisClient>();
    clients.insert(clientPtr);
    clientPtr->DoReq("GET A", 
    [&clients, clientPtr](small_server::RedisClient &client, 
        const std::string &errMsg) {
        clients.erase(clientPtr);
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
    return;
}

int main() {
    std::string errMsg;
    small_server::SheepNetCore::GetInstance()->Init();
    //small_net::AsioNet::GetInstance().Init();
    small_server::RedisCore::GetInstance()->Init(
    errMsg, {"172.16.187.149"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::set<std::shared_ptr<small_server::RedisClient>> clients;
    for (;;) {
        DoReq(errMsg, clients);
        if (!errMsg.empty()) {
            LOG(INFO) << errMsg;
            return -1;
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_server::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
