#include "small_server.h"
#include "small_client.h"
#include "small_watcher.h"
#include "helloworld.grpc.pb.h"
#include "helloworld.pb.h"
#include "log.h"
    
small_server::GrpcClientChannel<helloworld::Greeter> channel;

void Init(std::string &errMsg) {
    small_client::Looper::GetInstance()->Init();
    small_watcher::WatcherResolverFactory::GetInstance()->Init();
    channel.SetResolverType("watcher");
    channel.Init(errMsg, {"172.16.187.149"}, 2379, "/test");
}

void DoReq(std::string &errMsg) {
    using GrpcClientTest = small_server::GrpcClient<helloworld::HelloRequest, 
        helloworld::HelloReply, helloworld::Greeter>;
    auto client = std::make_shared<GrpcClientTest>(channel);
    client->Init();
    client->req_.set_name("proxy");
    client->DoReq([](GrpcClientTest &client, 
    const std::string &errMsg) {
        LOG(INFO) << client.resp_.message();
    });
}

int main() {
    small_server::GrpcLooper::GetInstance()->Init();
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
    std::this_thread::sleep_for(std::chrono::seconds(100));
}
