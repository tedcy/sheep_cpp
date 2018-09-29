#include "small_config_factory.h"
#include "log.h"

using namespace small_log;

int main() {
    auto config = small_config::MakeConfig();
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
