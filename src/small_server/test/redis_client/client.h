#pragma once
#include "small_server.h"
#include "log.h"

std::shared_ptr<small_server::RedisClient> DoReq(std::string &errMsg) {
    auto client = std::make_shared<small_server::RedisClient>();
    client->DoReq("GET A", 
    [](small_server::RedisClient &client, 
        const std::string &errMsg) {
        LOG(INFO) << "DoReq Done";
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        bool ok;
        auto resp = client.GetResp(ok);
        if (!ok) {
            LOG(WARNING) << "not exist";
            return;
        }
        LOG(INFO) << resp;
    });
    return client;
}
