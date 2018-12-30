#pragma once
#include "small_packages.h"
#include <map>
#include "balancer.h"
#include "log.h"

namespace small_client{
template <typename ClientPoolT>
class ClientManager: public small_packages::noncopyable{
public:
using MakeClientHandlerT = 
    std::function<std::shared_ptr<ClientPoolT>(std::string &errMsg,
    const std::string &addrPort)>;
    void Update(std::set<std::string> &nodes) {
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
    void SetMakeClientHandler(MakeClientHandlerT handler) {
        handler_ = handler;
    }
    std::shared_ptr<ClientPoolT> GetClientPool(bool &ok, const std::string &addr) {
        auto iter = clientPools_.find(addr);
        if (iter == clientPools_.end()) {
            ok = false;
            return nullptr;
        }
        ok = true;
        auto clientPool = iter->second;
        return clientPool;
    }
    void AddrPort2addrAndPort(bool &ok, const std::string &addrPort, std::string &addr, int &port) {
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
private:
    void addAddr(std::string &errMsg, const std::string &addrPort) {
        auto clientPool = handler_(errMsg, addrPort);
        if(!errMsg.empty()) {
            LOG(WARNING) << errMsg;
            return;
        }
        clientPools_.insert({addrPort, clientPool});
    }
    void removeAddr(const std::string &addrPort) {
        clientPools_.erase(addrPort);
    }
private:
    std::map<std::string, std::shared_ptr<ClientPoolT>> clientPools_;
    MakeClientHandlerT handler_;
};
}
