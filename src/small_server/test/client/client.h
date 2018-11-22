#pragma once
#include "small_server.h"
#include "helloworld.grpc.pb.h"
#include "helloworld.pb.h"
#include "log.h"

void DoReq(std::string &errMsg) {
    using GrpcClientTest = small_server::GrpcClient<helloworld::HelloRequest, 
        helloworld::HelloReply, helloworld::Greeter>;
    auto client = std::make_shared<GrpcClientTest>();
    client->Init();
    client->req_.set_name("proxy");
    client->DoReq("127.0.0.1:8888", 
            [](GrpcClientTest &client, 
                const std::string &errMsg) {
                LOG(INFO) << client.resp_.message();
            });
}
