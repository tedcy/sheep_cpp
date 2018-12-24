#pragma once
#include "client.h"
#include "event_loop.h"
#include <set>
#include <memory>
#include "small_packages.h"

namespace sheep{
namespace net{
//TODO thread unsafe
class ClientPool {
public:
    ClientPool(EventLoop &loop,
            const std::string &addr, int port,
            uint64_t maxSize);
    void Init(std::string &errMsg);
    std::shared_ptr<Client> Get();
    void Insert(std::shared_ptr<Client> client);
private:
    void initClient(std::string &errMsg,
            std::shared_ptr<Client> &client);
    EventLoop &loop_;
    const std::string addr_;
    int port_;
    uint64_t maxSize_ = 0;
    //composition
    std::set<std::shared_ptr<Client>> clients_;
    std::set<std::shared_ptr<Client>> allClients_;
    std::shared_ptr<bool> exist_;
    std::shared_ptr<small_lock::LockI> lock_;
    bool inited_ = false;
};
}
}
