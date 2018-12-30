#include "client.h"
#include "log.h"
#include "small_server.h"

int main() {
    std::string errMsg;
    small_server::SheepNetCore::GetInstance()->Init();
    //small_net::AsioNet::GetInstance().Init();
    small_server::SheepNetClientCore core(small_server::SheepNetCore::GetInstance()->GetLoop());
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
    small_server::SheepNetCore::GetInstance()->Shutdown();
    //small_net::AsioNet::GetInstance().Shutdown();
}
