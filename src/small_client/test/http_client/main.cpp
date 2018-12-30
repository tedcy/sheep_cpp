#include "log.h"
#include "small_client.h"

std::shared_ptr<small_client::HttpClient> DoReq(std::string &errMsg,
        small_client::SheepNetClientCore &core) {
    auto client = std::make_shared<small_client::HttpClient>(core,
            small_http_parser::ReqFormater::MethodGET, "127.0.0.1", "/", "");
    client->DoReq([](small_client::HttpClient &client, 
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

int main() {
    std::string errMsg;
    small_client::SheepNetCore::GetInstance()->Init();
    //small_net::AsioNet::GetInstance().Init();
    small_client::SheepNetClientCore core(
            small_client::SheepNetCore::GetInstance()->GetLoop());
    core.SetResolverType("string");
    core.SetMaxSize(10);
    core.Init(errMsg, {"127.0.0.1"}, 80, "");
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto hold = DoReq(errMsg, core);
    if (!errMsg.empty()) {
        LOG(INFO) << errMsg;
        return -1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(100));
    small_client::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
