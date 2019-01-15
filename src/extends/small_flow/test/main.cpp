#include "service.h"
#include "log.h"
#include "req_filling.h"
#include "req_forward.h"
#include "resp_filling.h"
    
small_server::GrpcClientChannel<helloworld::Greeter> channel;

int main() {
    small_server::GrpcServer server("127.0.0.1:8888");
    server.Init();

    test_service::TestService service;
    service.Init();
    service.AddPloy(new (test_service::ploy::ReqFilling));
    service.AddPloy(new (test_service::ploy::ReqForward));
    service.AddPloy(new (test_service::ploy::RespFilling));
    server.Register(&service);

    std::string errMsg;
    channel.SetResolverType("string");
    channel.Init(errMsg, {"127.0.0.1"}, 8888, "");
    if(!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }

    server.Run();
}
