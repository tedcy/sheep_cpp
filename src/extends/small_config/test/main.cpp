#include "small_config_factory.h"
#include "log.h"

int main() {
    auto config = small_config::MakeConfig();
    config->AddKey("Addr");
    config->AddKey("Port");
    std::string errMsg;
    config->Load("test.toml", errMsg);
    if(errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    std::string Addr;
    uint32_t Port;
    Addr = config->GetString("Addr", errMsg);
    if (errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    Port = config->GetInt("Port", errMsg);
    if (errMsg != "") {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << Addr;
    LOG(INFO) << Port;
}
