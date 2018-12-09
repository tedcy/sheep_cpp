#include "client_pool.h"
#include "log.h"

namespace sheep{
namespace net{
ClientPool::ClientPool(EventLoop &loop,
            const std::string &addr, int port,
            uint64_t maxSize) :
    loop_(loop),
    addr_(addr), port_(port),
    maxSize_(maxSize), 
    exist_(std::make_shared<bool>()){
}

void ClientPool::Init(std::string &errMsg) {
    for (uint64_t i = 0;i < maxSize_;i++) {
        auto client = std::make_shared<Client>(loop_, addr_, port_);
        std::weak_ptr<bool> weakExist = exist_;
        std::weak_ptr<Client> weakClient = client;
        client->SetConnectedHandler([weakExist, this, weakClient](
        const std::string &argErrMsg) {
            if (!argErrMsg.empty()) {
                LOG(WARNING) << argErrMsg;
                return;
            }
            auto client = weakClient.lock();
            if (!client) {
                LOG(WARNING) << 
                    "set disconnect handler failed: Client has been destoryed";
                return;
            }
            auto exist = weakExist.lock();
            if (!exist) {
                LOG(WARNING) << 
                    "set disconnect handler failed: ClientPool has been destoryed";
                return;
            }
            Insert(client);
        });
        client->SetDisconnectedHandler([weakExist, this, weakClient](
        const std::string &argErrMsg){
            if (!argErrMsg.empty()) {
                LOG(WARNING) << argErrMsg;
                return;
            }
            auto client = weakClient.lock();
            if (!client) {
                LOG(WARNING) << 
                    "set disconnect handler failed: Client has been destoryed";
                return;
            }
            auto exist = weakExist.lock();
            if (!exist) {
                LOG(WARNING) << 
                    "set disconnect handler failed: ClientPool has been destoryed";
                return;
            }
            std::string errMsg;
            initClient(errMsg, client);
            if (!errMsg.empty()) {
                LOG(WARNING) << 
                    "reconnect failed: " + errMsg;
                return;
            }
        });
        initClient(errMsg, client);
        if (!errMsg.empty()) {
            return;
        }
    }
}

void ClientPool::initClient(std::string &errMsg,
        std::shared_ptr<Client> &client) {
    //earse for reconnect
    clients_.erase(client);
    allClients_.insert(client);
    client->AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        return;
    }
}

std::shared_ptr<Client> ClientPool::Get() {
    if (clients_.empty()) {
        return nullptr;
    }
    auto iter = clients_.begin();
    clients_.erase(iter);
    return *iter;
}

void ClientPool::Insert(std::shared_ptr<Client> client) {
    clients_.insert(client);
}
}
}
