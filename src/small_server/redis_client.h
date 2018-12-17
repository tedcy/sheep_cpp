#pragma once
#include <functional>
#include <string>
#include "net.h"
#include "small_hiredis.h"
#include "small_packages.h"
#include "log.h"
#include "redis_core.h"
namespace small_server{
class RedisClient: public small_packages::noncopyable{
public:
using RedisClientOnDone = 
    std::function<void(RedisClient& c, const std::string &errMsg)>;
    RedisClient() {
    }
    void DoReq(const std::string &command, RedisClientOnDone onDone) {
        command_ = command;
        doReq(onDone);
    }
    ~RedisClient() {
        if (cmdBuf_ != nullptr) {
            delete cmdBuf_;
        }
        if (reader_ != nullptr) {
            redisReaderFree(reader_);
        }
    }
    std::string GetResp(bool &ok) {
        ok = ok_;
        return value_;
    }
protected:
    template <typename T>
    void doReq(std::function<void(T&, const std::string&)> &onDone) {
        auto realOnDone = [onDone](RedisClient &c,const std::string &errMsg) {
            T &t = dynamic_cast<T&>(c);
            onDone(t, errMsg);
        };
        onDone_ = realOnDone;
        bool ok;
        clientPool_ = RedisCore::GetInstance()->GetClientPool(ok);
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
            onDone_(*this, errMsg);
            return;
        }
        connection.AsyncWrite([this](std::string &errMsg){
            if(!errMsg.empty()) {
                onDone_(*this, errMsg);
            }
            reader_ = redisReaderCreate();
            auto &connection = client_->GetTcpConnection();
            connection.AsyncReadAny([this](std::string &errMsg) {
                if(!errMsg.empty()) {
                    onDone_(*this, errMsg);
                }
                auto &connection = client_->GetTcpConnection();
                bool finish;
                RespPop(errMsg, finish, connection);
                if(!errMsg.empty()) {
                    onDone_(*this, errMsg);
                }
                if (finish) {
                    onDone_(*this, "");
                    clientPool_->Insert(client_);
                }
            });
        });
    }
private:
    void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) {
        cmdBufLen_ = redisFormatCommand(&cmdBuf_, command_.c_str());
        if (cmdBufLen_ < 0) {
            errMsg = "redisFormatCommand error! len = " + std::to_string(cmdBufLen_);
            return;
        }
        connection.WriteBuffer_.Push(cmdBuf_, cmdBufLen_);
    }
    void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) {
        for (;;) {
            char tmpBuf[1024];
            uint64_t len;
            len = connection.ReadBuffer_.PopHead(tmpBuf, 1024);
            if (len == 0) {
                break;
            }
            redisReaderFeed(reader_, tmpBuf, len);
        }
        auto ret = redisReaderGetReply(reader_, &reply_);
        if (ret != REDIS_OK) {
            errMsg = "redisReaderGetReply error: " + std::to_string(ret);
            finish = true;
            return;
        }
 
        if (reply_ == nullptr) {
            //wait for next time
            return;
        }
        finish = true;
        struct redisReply *r = (struct redisReply*)reply_;
        if (REDIS_REPLY_ERROR == r->type) {
            errMsg = "redisReply return REDIS_REPLY_ERROR";
            return;
        }
        if (r->type == REDIS_REPLY_NIL) {
            ok_ = false;
            return;
        }
        value_.append(r->str, r->len);
    }
    //req
    std::string command_;
    char *cmdBuf_ = nullptr;
    int64_t cmdBufLen_ = 0;
    //resp
    redisReader *reader_ = nullptr;
    void *reply_ = nullptr;
    std::string value_;
    bool ok_ = true;

    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    RedisClientOnDone onDone_;
};
}
