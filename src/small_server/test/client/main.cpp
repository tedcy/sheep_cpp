#include "client.h"
#include "log.h"
#include "small_server.h"

int main() {
    small_server::GrpcCore::GetInstance()->Init();
    small_server::GrpcCore::GetInstance()->Run();
    std::string errMsg;
    DoReq(errMsg);
    if (errMsg != "") {
        LOG(INFO) << errMsg;
    }
    LOG(INFO) << "ok";
    small_server::GrpcCore::GetInstance()->Wait();
}
