#pragma once
#include "log.h"
#include "net.h"
#include "small_packages.h"
#include <map>
#include "balancer.h"
namespace small_server{
class SheepNetClientCore: public small_packages::noncopyable{
public:
    SheepNetClientCore(sheep::net::EventLoop &loop):
        loop_(loop) {
    }
    virtual ~SheepNetClientCore() = default;
    void SetMaxSize(const uint64_t maxSize) {
        maxSize_ = maxSize;
    }
    void SetLbPolicyType(const std::string &LbPolicyType) {
        lbPolicyType_ = LbPolicyType;
    }
    void SetResolverType(const std::string &ResolverType) {
        resolverType_ = ResolverType;
    }
    void Init(std::string &errMsg, const std::vector<std::string> &ips, uint32_t port, 
            const std::string &target) {
        balancer_ = std::unique_ptr<Balancer>(new Balancer(ips, port, target, lbPolicyType_, resolverType_));
        balancer_->Init(errMsg, [this](std::set<std::string> &nodes){
            update(nodes);
        });
    }
    std::shared_ptr<sheep::net::ClientPool> GetClientPool(bool &ok) {
        auto addr = balancer_->Get(ok);
        if (!ok) {
            return nullptr;
        }
        auto iter = clientPools_.find(addr);
        if (iter == clientPools_.end()) {
            ok = false;
            return nullptr;
        }
        ok = true;
        auto clientPool = iter->second;
        return clientPool;
    }
private:
    void update(std::set<std::string> &nodes) {
        std::vector<std::string> needAdd;
        std::vector<std::string> needRemove;
        for (auto &node: nodes) {
            auto iter = clientPools_.find(node);
            if(iter == clientPools_.end()) {
                needAdd.push_back(node);
            }
        }
        for (auto &pair: clientPools_) {
            auto iter = nodes.find(pair.first);
            if (iter == nodes.end()) {
                needRemove.push_back(pair.first);
            }
        }
        for (auto &node: needAdd) {
            std::string errMsg;
            addAddr(errMsg, node);
            if (!errMsg.empty()) {
                LOG(WARNING) << errMsg;
            }
        }
        for (auto &node: needRemove) {
            removeAddr(node);
        }
    }
    void addrPort2addrAndPort(bool &ok, const std::string &addrPort, std::string &addr, int &port) {
        auto list = small_strings::split(addrPort, ':');
        if (!list) {
            ok = false;
            return;
        }
        if (list->empty()) {
            ok = false;
            return;
        }
        port = -1;
        port = std::stoi(*list->rbegin());
        if (port == -1) {
            ok = false;
        }
        ok = true;
        addr = *list->begin();
    }
    void addAddr(std::string &errMsg, const std::string &addrPort) {
        std::string addr;
        int port;
        bool ok;
        addrPort2addrAndPort(ok, addrPort, addr, port);
        if (!ok) {
            errMsg = "invalid addPort " + addrPort; 
            return;
        }
        auto clientPool = std::make_shared<sheep::net::ClientPool>(loop_, addr, port, maxSize_);
        clientPool->Init(errMsg);
        if(!errMsg.empty()) {
            return;
        }
        clientPools_.insert({addr + ":" + std::to_string(port), clientPool});
    }
    void removeAddr(const std::string &addrPort) {
        clientPools_.erase(addrPort);
    }
    std::map<std::string, std::shared_ptr<sheep::net::ClientPool>> clientPools_;
    sheep::net::EventLoop &loop_;
    uint64_t maxSize_ = 50;
    std::unique_ptr<Balancer> balancer_;
    std::string lbPolicyType_ = "random";
    std::string resolverType_ = "etcd";
};
}
