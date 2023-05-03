#pragma once
#include "resolver_interface.h"
#include "extends/small_packages/small_packages.h"
#include "log/log.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
namespace small_client{
namespace resolver{
class DefaultDns {
public:
    static void Resolver(std::string &errMsg,
            const std::string &host, std::set<std::string> &addrs) {
        struct hostent *hptr;
        hptr = gethostbyname(host.c_str());
        if (hptr == nullptr) {
            errMsg = "gethostbyname error for host: " + host + hstrerror(h_errno);
            return;
        }
        char **pptr;
		char str[INET_ADDRSTRLEN];
		switch (hptr->h_addrtype) {
        	case AF_INET:
            	pptr = hptr->h_addr_list;
            	for (; *pptr != nullptr; pptr++) {
					const char *c = inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));
					addrs.insert(std::string(c, std::strlen(c)));
            	}
            	break;
        	default:
            	break;
    	}
        if (addrs.empty()) {
            errMsg = host + " no available dns addrs";
        }
    }
};
class DnsResolver: public ResolverI{
public:
    DnsResolver(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) :
        ips_(ips), port_(port), target_(target){
    }
    void Init(std::string &errMsg) override {
        blockQueue_.Init();
    }
    void SetNotifyWatcherChange(WatcherChangeHandlerT handler) override {
        handler_ = handler;
        resolveOnce(true);
    }
private:
    void resolveOnce(bool first) {
        std::string errMsg;
        std::set<std::string> nodes;
        for (auto &ip: ips_) {
            DefaultDns::Resolver(errMsg, ip, nodes);
            if (!errMsg.empty()) {
                LOG(WARNING) << errMsg;
            }
        }
        std::set<std::string> addrPortNodes;
        for (auto &node: nodes) {
            addrPortNodes.insert(node + (":" + std::to_string(port_)));
        }
        if (!nodes.empty()) {
            handler_(addrPortNodes);
        }
        if (first) {
            //not in thread
            blockQueue_.Push([this](){
                resolveOnce(false);
            });
        }else {
            //in thread
            blockQueue_.PushWithoutLock([this](){
                resolveOnce(false);
            });
            std::this_thread::sleep_for(std::chrono::seconds(100));
        }
    }
    std::vector<std::string> ips_;
    uint32_t port_;
    std::string target_;
    WatcherChangeHandlerT handler_;
    small_block_queue::BlockQueue blockQueue_;
};

class DnsResolverFactory: public ResolverFactoryI{
public:
    static DnsResolverFactory* GetInstance() {
        static DnsResolverFactory instance;
        return &instance;
    }
    std::unique_ptr<ResolverI> Create(const std::vector<std::string> &ips, uint32_t port,
            const std::string &target) override {
        return std::unique_ptr<ResolverI>(new DnsResolver(ips, port, target));
    }
private:
    DnsResolverFactory() = default;
};
}
}
