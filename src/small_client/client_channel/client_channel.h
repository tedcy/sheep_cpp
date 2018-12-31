#pragma once
#include "log.h"
#include "net.h"
#include "small_packages.h"
#include "balancer.h"
#include "client_manager.h"
namespace small_client{
class ClientChannel: public small_packages::noncopyable{
public:
    ClientChannel(sheep::net::EventLoop &loop):
        loop_(loop) {
    }
    virtual ~ClientChannel() = default;
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
        auto weakPtr = std::weak_ptr<bool>(exist_);
        clientManager_.SetMakeClientHandler([weakPtr, this](std::string &errMsg,
                    const std::string &addrPort){
            std::shared_ptr<sheep::net::ClientPool> clientPool;
            auto exist = weakPtr.lock();
            if(!exist) {
                LOG(WARNING) << "ClientChannel destoryed";
                return clientPool;
            }
            bool ok;
            std::string addr;
            int port;
            clientManager_.AddrPort2addrAndPort(ok, addrPort, addr, port);
            clientPool = std::make_shared<sheep::net::ClientPool>(loop_, addr, port, maxSize_);
            clientPool->Init(errMsg);
            if (!errMsg.empty()) {
                clientPool = nullptr;
                return clientPool;
            }
            return clientPool;
        });
        balancer_ = std::unique_ptr<Balancer>(new Balancer(ips, port, target, lbPolicyType_, resolverType_));
        balancer_->Init(errMsg, [this](std::set<std::string> &nodes){
            clientManager_.Update(nodes);
        });
    }
    std::shared_ptr<sheep::net::ClientPool> GetClientPool(bool &ok) {
        auto addr = balancer_->Get(ok);
        if (!ok) {
            return nullptr;
        }
        return clientManager_.GetClientPool(ok, addr);
    }
private:
    std::shared_ptr<bool> exist_ = std::make_shared<bool>();
    ClientManager<sheep::net::ClientPool> clientManager_;
    sheep::net::EventLoop &loop_;
    uint64_t maxSize_ = 50;
    std::unique_ptr<Balancer> balancer_;
    std::string lbPolicyType_ = "random";
    std::string resolverType_ = "string";
};
}
