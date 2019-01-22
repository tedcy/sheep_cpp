#pragma once
#include <functional>
#include <string>
#include "net.h"
#include "small_hiredis.h"
#include "small_packages.h"
#include "log.h"
#include "base_client.h"
namespace small_client{
class RedisClient : public BaseClient{
public:
using RedisClientOnDone = 
    std::function<void(RedisClient& c, const std::string &errMsg)>;
    RedisClient(ClientChannel &channel) :
        BaseClient(channel) ,
        reader_(redisReaderCreate()){
    }
    void DoReq(const std::string &command, RedisClientOnDone onDone) {
        command_ = command;
        doReq<RedisClient>(onDone);
    }
    ~RedisClient() {
        if (cmdBuf_ != nullptr) {
            delete cmdBuf_;
        }
        if (reader_ != nullptr) {
            redisReaderFree(reader_);
        }
        if (reply_ != nullptr) {
            freeReplyObject(reply_);
        }
    }
    void GetResp(bool &ok, std::string &resp) {
        if (respType_ != REDIS_REPLY_STRING) {
            ok = false;
            return;
        }
        ok = true;
        resp = value_;
    }
    void GetResp(bool &ok, std::vector<std::string> &resp) {
        if (respType_ != REDIS_REPLY_ARRAY) {
            ok = false;
            return;
        }
        ok = true;
        resp = arrayValues_;
    }
private:
    void ReqPush(std::string &errMsg, sheep::net::TcpConnection &connection) override {
        cmdBufLen_ = redisFormatCommand(&cmdBuf_, command_.c_str());
        if (cmdBufLen_ < 0) {
            errMsg = "redisFormatCommand error! len = " + std::to_string(cmdBufLen_);
            return;
        }
        connection.WriteBufferPush(cmdBuf_, cmdBufLen_);
    }
    void RespPop(std::string &errMsg, bool &finish, sheep::net::TcpConnection &connection) override{
        for (;;) {
            char tmpBuf[1024];
            uint64_t len;
            len = connection.ReadBufferPopHead(tmpBuf, 1024);
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
        respType_ = r->type;
        if (r->type == REDIS_REPLY_NIL) {
            return;
        }
        switch (r->type) {
        case REDIS_REPLY_ARRAY:
            for (int i = 0;i < r->elements;i++) {
                arrayValues_.push_back(r->element[i]->str);
            }
            break;
        default:
            value_.append(r->str, r->len);
        }
    }
    //req
protected:
    std::string command_;
private:
    char *cmdBuf_ = nullptr;
    int64_t cmdBufLen_ = 0;
    //resp
    redisReader *reader_ = nullptr;
    void *reply_ = nullptr;
    std::string value_;
    std::vector<std::string> arrayValues_;
    int respType_ = 0;

    std::shared_ptr<sheep::net::ClientPool> clientPool_;
    std::shared_ptr<sheep::net::Client> client_;
    RedisClientOnDone onDone_;
};
template<typename ServiceEventT>
class RedisClientWithService: public RedisClient{
public:
    RedisClientWithService(ClientChannel &channel):
        RedisClient(channel) {
    }
    void DoReq(const std::string &command, 
            std::function<void(RedisClientWithService&, const std::string&)> onDone) {
        command_ = command;
        //FIXME: auto is invliad, wtf need this line???
        std::function<void(RedisClientWithService&, const std::string&)> realOnDone;
        realOnDone = [this, onDone](RedisClientWithService&, const std::string &errMsg) {
            auto realEvent = serviceEvent_.lock();
            if (!realEvent) {
                return;
            }
            auto lock = realEvent->GetLock();
            small_lock::UniqueGuard uniqueLock (lock);
            onDone(*this, errMsg);
        };
        this->doReq<RedisClientWithService>(realOnDone);
    }
    std::weak_ptr<ServiceEventT> GetServiceEvent() {
        return serviceEvent_;
    }
private:
    friend ServiceEventT;
    void SetServiceEvent(std::shared_ptr<ServiceEventT> serviceEvent) {
        serviceEvent_ = serviceEvent;
    }
    //association
    std::weak_ptr<ServiceEventT> serviceEvent_;
};
}
