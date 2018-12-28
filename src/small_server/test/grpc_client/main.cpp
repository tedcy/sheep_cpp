#include "client.h"
#include "log.h"
#include "small_server.h"

int main() {
    small_server::GrpcCore::GetInstance()->Init();
    small_server::GrpcCore::GetInstance()->Run();
    std::string errMsg;
    Init(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    DoReq(errMsg);
    if (errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << "ok";
    small_server::GrpcCore::GetInstance()->Wait();
}
