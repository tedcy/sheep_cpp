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
    lock_(small_lock::MakeRecursiveLock()){
}

ClientPool::~ClientPool() {
    //LOG(DEBUG) << "~ClientPool";
}

void ClientPool::Init(std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    if (inited_) {
        errMsg = "ClientPool has been inited";
        return;
    }
    inited_ = true;
    for (uint64_t i = 0;i < maxSize_;i++) {
        initClient(errMsg);
        if (!errMsg.empty()) {
            LOG(WARNING) << 
                "connect failed: " + errMsg;
            return;
        }
    }
}

void ClientPool::initClient(std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    auto client = std::make_shared<Client>(loop_, addr_, port_);
    std::weak_ptr<ClientPool> weakThis = shared_from_this();
    std::weak_ptr<Client> weakClient = client;
    client->SetConnectedHandler([this, weakThis, weakClient](
    const std::string &argErrMsg) {
        if (!argErrMsg.empty()) {
            LOG(WARNING) << argErrMsg;
            return;
        }
        auto client = weakClient.lock();
        if (!client) {
            LOG(WARNING) << 
                "connect handler failed: Client has been destoryed";
            return;
        }
        auto realThis = weakThis.lock();
        if (!realThis) {
            LOG(WARNING) << 
                "connect handler failed: ClientPool has been destoryed";
            return;
        }
        Insert(client, "");
    });
    client->SetDisconnectedHandler([this, weakThis, weakClient](
    const std::string &argErrMsg){
        //when argErrMsg != "", means err happened, should reconnect
        if (argErrMsg.empty()) {
            return;
        }
        LOG(WARNING) << argErrMsg;
        auto realThis = weakThis.lock();
        if (!realThis) {
            LOG(WARNING) << 
                "disconnect handler failed: ClientPool has been destoryed";
            return;
        }
        std::string errMsg;
        initClient(errMsg);
        if (!errMsg.empty()) {
            LOG(WARNING) << 
                "reconnect failed: " + errMsg;
        }
        small_lock::UniqueGuard guard(lock_);
        auto client = weakClient.lock();
        if (!client) {
            LOG(WARNING) << 
                "disconnect handler failed: Client has been destoryed";
            return;
        }
        //client in pool closed by peer
        clients_.erase(client);
        allClients_.erase(client);
        
    });
    allClients_.insert(client);
    client->AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        return;
    }
}

std::shared_ptr<Client> ClientPool::Get() {
    small_lock::UniqueGuard guard(lock_);
    if (clients_.empty()) {
        return nullptr;
    }
    auto iter = clients_.begin();
    auto client = *iter;
    clients_.erase(iter);
    return client;
}

void ClientPool::Insert(std::shared_ptr<Client> client,
    const std::string &errMsg) {
    small_lock::UniqueGuard guard(lock_);
    client->Reset(errMsg);
    if (!errMsg.empty()) {
        return;
    }
    clients_.insert(client);
}
}
}
