#include "small_config_factory.h"
#include "log.h"

int main() {
    auto config = small_config::MakeConfig();
    std::string errMsg;
    config->Load("test.toml", errMsg);
    if(errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    std::string Addr;
    uint64_t Port;
    config->Get(errMsg, "Addr", Addr);
    if (errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    config->Get(errMsg, "Port", Port);
    if (errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << Addr;
    LOG(INFO) << Port;
}
