#pragma once
#include "service.h"
#include <memory>
class ServiceFactory {
public:
    virtual ~ServiceFactory() = default;
    virtual Service* CreateService() = 0;
};
