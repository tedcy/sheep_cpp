#pragma once
#include "../../server/service/service.h"
#include "../../server/service/service_factory.h"
#include "../../server/event_loop/event_loop.h"
#include <memory>

class RealServiceFactory: public ServiceFactory{
    public:
        RealServiceFactory() = default;
        ~RealServiceFactory() = default;
        Service* CreateService() override{
            return new RealService();
        }
};
