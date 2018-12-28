#pragma once
#include <functional>
#include <string>
#include "small_packages.h"
#include "log.h"
#include "sheep_net_client_core.h"
namespace small_server{
class NetClient: public small_packages::noncopyable{
public:
using NetClientOnDone = 
    std::function<void(NetClient& c, const std::string &errMsg)>;
    NetClient(SheepNetClientCore &core) :
        sheepNetClientCore_(core){
    }
    virtual ~NetClient() = default;
    virtual void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) = 0;
    virtual void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) = 0;
protected:
    template <typename T>
    void doReq(std::function<void(T&, const std::string&)> &onDone) {
        auto realOnDone = [onDone](NetClient &c,const std::string &errMsg) {
            T &t = dynamic_cast<T&>(c);
            onDone(t, errMsg);
        };
        onDone_ = realOnDone;
        bool ok;
        clientPool_ = sheepNetClientCore_.GetClientPool(ok);
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
    SheepNetClientCore &sheepNetClientCore_;
    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    NetClientOnDone onDone_;
};
}
