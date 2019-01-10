#include "client.h"
#include "log.h"
#include "small_client.h"
#include "small_packages.h"
#include "small_pprof.h"

void DoReq(std::string &errMsg, small_client::ClientChannel &channel,
        std::set<std::shared_ptr<small_client::RedisClient>> &clients,
        std::shared_ptr<small_lock::LockI> &lock) {
    small_lock::UniqueGuard guard(lock);
    if (clients.size() >= 50) {
        return;
    }
    auto clientPtr = std::make_shared<small_client::RedisClient>(channel);
    auto weakPtr = std::weak_ptr<small_client::RedisClient>(clientPtr);
    clients.insert(clientPtr);
    clientPtr->SetTimeoutMs(5000);
    clientPtr->DoReq("GET A", 
    [&clients, weakPtr, &lock](small_client::RedisClient &client, 
        const std::string &errMsg) {
        small_lock::UniqueGuard guard(lock);
        auto clientPtr = weakPtr.lock();
        if (!clientPtr) {
            LOG(FATAL) << "weakPtr is nullptr";
        }
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            clients.erase(clientPtr);
            return;
        }
        bool ok;
        auto resp = client.GetResp(ok);
        if (!ok) {
            LOG(WARNING) << "not exist";
            clients.erase(clientPtr);
            return;
        }
        //LOG(INFO) << resp;
        clients.erase(clientPtr);
    });
    return;
}

int main() {
    ghttpd();
    std::string errMsg;
    small_client::Looper::GetInstance()->Init();
    small_client::ClientChannel channel(
            small_client::Looper::GetInstance()->GetLoop());
    channel.SetResolverType("string");
    channel.SetMaxSize(60);
    channel.Init(errMsg, {"127.0.0.1"}, 6379, "/");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::set<std::shared_ptr<small_client::RedisClient>> clients;
    auto lock = small_lock::MakeRecursiveLock();
    for (;;) {
        DoReq(errMsg, channel, clients, lock);
        if (!errMsg.empty()) {
            LOG(INFO) << errMsg;
            return -1;
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_client::Looper::GetInstance()->Shutdown();
}
