#pragma once
#include "small_server.h"
#include "log.h"

std::shared_ptr<small_server::HttpClientBackUp> DoReq(std::string &errMsg,
        small_server::SheepNetClientCore &core) {
    auto client = std::make_shared<small_server::HttpClientBackUp>(core,
            small_http_parser::ReqFormater::MethodGET, "127.0.0.1", "/", "");
    client->DoReq([](small_server::HttpClientBackUp &client, 
        const std::string &errMsg) {
        LOG(INFO) << "DoReq Done";
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        auto headers = client.GetRespHeader();
        for (auto &header: headers.kvs_) {
            LOG(INFO) << header.first << ":" << header.second;
        }
        auto resp = client.GetRespStr();
        LOG(INFO) << resp;
    });
    return client;
}
