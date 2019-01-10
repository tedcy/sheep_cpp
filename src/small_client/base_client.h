#pragma once
#include <functional>
#include <string>
#include "small_packages.h"
#include "log.h"
#include "client_channel/client_channel.h"
#include "looper.h"
namespace small_client{
class BaseClient: public small_packages::noncopyable{
public:
using BaseClientOnDone = 
    std::function<void(BaseClient& c, const std::string &errMsg)>;
    BaseClient(ClientChannel &core) :
        clientChannel_(core){
    }
    virtual ~BaseClient() {
        //LOG(DEBUG) << "~BaseClient";
    }
    virtual void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) = 0;
    virtual void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) = 0;
    void SetTimeoutMs(uint64_t ms) {
        timeoutMs_ = ms;
    }
protected:
    const std::string& GetAddr() {
        return client_->GetAddr();
    }
    int GetPort() {
        return client_->GetPort();
    }
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
        if (timeoutMs_ != 0) {
            timer_ = std::make_shared<sheep::net::Timer>(
                Looper::GetInstance()->GetLoop());
            auto weakTimer= std::weak_ptr<sheep::net::Timer>(timer_);
            timer_->AsyncWait(timeoutMs_, [this, weakTimer](const std::string &errMsg){
                auto timer = weakTimer.lock();
                if (!timer) {
                    return;
                }
                if (!errMsg.empty()) {
                    return;
                }
                Finish("timeout");
            });
        }
        connection.AsyncWrite([this](const std::string &errMsg){
            if(!errMsg.empty()) {
                Finish(errMsg);
                return;
            }
            auto &connection = client_->GetTcpConnection();
            connection.AsyncReadAny([this](const std::string &argErrMsg) {
                if (timer_ != nullptr) {
                    timer_->Cancel();
                }
                if(!argErrMsg.empty()) {
                    Finish(argErrMsg);
                    return;
                }
                std::string errMsg;
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
        if (finished_) {
            //LOG(FATAL) << "base client finished";
            return;
        }
        finished_ = true;
        auto clientPool = clientPool_;
        auto client = client_;
        onDone_(*this, errMsg);
        clientPool->Insert(client, errMsg);
    }
    ClientChannel &clientChannel_;
    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    uint64_t timeoutMs_ = 0;
    BaseClientOnDone onDone_;
    std::shared_ptr<sheep::net::Timer> timer_;
    bool finished_ = false;
};
}
