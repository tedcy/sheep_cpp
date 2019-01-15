#include "service.h"
#include "small_watcher.h"
#include "log.h"
    
small_server::GrpcClientChannel<helloworld::Greeter> channel;

int main() {
    small_server::GrpcServer server("192.168.2.128:8888");
    server.Init();

    test_service::TestService service;
    service.Init();
    server.Register(&service);

    //Server part
    std::string errMsg;
    //初始化small_watcher所需的small_client::Looper
    small_client::Looper::GetInstance()->Init();
    auto watcher = small_watcher::MakeWatcher({"172.16.187.149"}, 2379);
    watcher->Init(errMsg);
    if(!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    std::string ip;
    watcher->GetLocalIp(ip);
    watcher->CreateEphemeral("/test/" + ip + ":8888", "");

    //Client part
    small_watcher::WatcherResolverFactory::GetInstance()->Init();
    channel.SetResolverType("watcher");
    channel.Init(errMsg, {"172.16.187.149"}, 2379, "/test");
    if(!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }

    server.Run();
}
