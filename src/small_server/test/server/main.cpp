#include "service.h"
#include "small_net.h"
#include "log.h"

int main() {
    small_net::AsioNet::GetInstance().Init();
    small_server::GrpcServer server("192.168.2.128:8888");
    server.Init();

    test_service::TestService service;
    service.Init();
    server.Register(&service);

    std::string errMsg;
    auto watcher = small_watcher::MakeWatcher({"172.16.187.149"}, 2379);
    watcher->Init(errMsg);
    if(!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    std::string ip;
    watcher->GetLocalIp(ip);
    watcher->CreateEphemeral("/test/" + ip + ":8888", "");

    //FIXME when here etcd, failed
    small_server::GrpcClientCore<helloworld::Greeter>::
        GetInstance()->SetResolverType("string");
    small_server::GrpcClientCore<helloworld::Greeter>::
        GetInstance()->Init(errMsg, {"192.168.2.128"}, 8888, "");
    server.Run();
}
