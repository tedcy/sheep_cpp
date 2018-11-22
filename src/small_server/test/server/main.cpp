#include "service.h"

int main() {
    small_server::GrpcServer server("127.0.0.1:8888");
    server.Init();

    test_service::TestService service;
    service.Init();
    server.Register(&service);

    server.Run();
}
