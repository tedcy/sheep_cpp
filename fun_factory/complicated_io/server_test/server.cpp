#include "../server/server.h"
#include "service/real_service.h"
#include "service/real_service_factory.h"
#include <memory>

int main() {
    //TODO 这里怎么用unique_pt
    std::shared_ptr<ServiceFactory> serviceFactory = 
        std::make_shared<RealServiceFactory>();
    std::shared_ptr<Server> server =
        std::make_shared<Server>(serviceFactory);
    if (server != nullptr) {
        if (server->Init((char*)"127.0.0.1", 12345) < 0) {
            return -1;
        }
        return server->Serve();
    }
    return 0;
}
