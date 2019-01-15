#pragma once
#include "log.h"
#include "small_packages.h"
#include "client_channel/balancer.h"
#include "client_channel/client_manager.h"

#include <grpcpp/grpcpp.h>

namespace small_server{
template<typename StubManager>
class GrpcClientChannel: public small_packages::noncopyable {
public:
    GrpcClientChannel() {
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
            std::shared_ptr<typename StubManager::Stub> stub;
            auto exist = weakPtr.lock();
            if(!exist) {
                LOG(WARNING) << "GrpcClientChannel destoryed";
                return stub;
            }
            stub = StubManager::NewStub(
                grpc::CreateChannel(addrPort, grpc::InsecureChannelCredentials()));
            return stub;
        });
        balancer_ = std::unique_ptr<small_client::Balancer>(
                new small_client::Balancer(ips, port, target, lbPolicyType_, resolverType_));
        balancer_->Init(errMsg, [this](std::set<std::string> &nodes){
            clientManager_.Update(nodes);
        });
    }
    std::shared_ptr<typename StubManager::Stub> GetClientPool(bool &ok) {
        auto addr = balancer_->Get(ok);
        if (!ok) {
            return nullptr;
        }
        return clientManager_.GetClientPool(ok, addr);
    }
private:
    std::shared_ptr<bool> exist_ = std::make_shared<bool>();
    small_client::ClientManager<typename StubManager::Stub> clientManager_;
    std::unique_ptr<small_client::Balancer> balancer_;
    std::string lbPolicyType_ = "random";
    std::string resolverType_ = "string";
};
}
