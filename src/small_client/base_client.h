#pragma once
#include <functional>
#include <string>
#include "small_packages.h"
#include "log.h"
#include "client_channel/client_channel.h"
namespace small_client{
class BaseClient: public small_packages::noncopyable{
public:
using BaseClientOnDone = 
    std::function<void(BaseClient& c, const std::string &errMsg)>;
    BaseClient(ClientChannel &core) :
        clientChannel_(core){
    }
    virtual ~BaseClient() = default;
    virtual void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) = 0;
    virtual void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) = 0;
protected:
    template <typename T>
    void doReq(std::function<void(T&, const std::string&)> &onDone) {
        auto realOnDone = [onDone](BaseClient &c,const std::string &errMsg) {
            T &t = dynamic_cast<T&>(c);
            onDone(t, errMsg);
        };
        onDone_ = realOnDone;
        bool ok;
        clientPool_ = clientChannel_.GetClientPool(ok);
        if (!ok) {
            onDone_(*this, "clientPool nullptr");
            return;
        }
        client_ = clientPool_->Get();
        if (!client_) {
            onDone_(*this, "client nullptr");
            return;
        }
        std::string errMsg;
        auto &connection = client_->GetTcpConnection();
        ReqPush(errMsg, connection);
        if (!errMsg.empty()) {
            Finish(errMsg);
            return;
        }
        connection.AsyncWrite([this](std::string &errMsg){
            if(!errMsg.empty()) {
                Finish(errMsg);
                return;
            }
            auto &connection = client_->GetTcpConnection();
            connection.AsyncReadAny([this](std::string &errMsg) {
                if(!errMsg.empty()) {
                    Finish(errMsg);
                    return;
                }
                auto &connection = client_->GetTcpConnection();
                bool finish;
                RespPop(errMsg, finish, connection);
                if(!errMsg.empty()) {
                    Finish(errMsg);
                    return;
                }
                if (finish) {
                    Finish("");
                }
            });
        });
    }
private:
    void Finish(const std::string &errMsg) {
        auto clientPool = clientPool_;
        auto client = client_;
        onDone_(*this, errMsg);
        clientPool->Insert(client);
    }
    ClientChannel &clientChannel_;
    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    BaseClientOnDone onDone_;
};
}
