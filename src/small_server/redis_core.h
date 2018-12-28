#pragma once
#include "sheep_net_core.h"
#include "sheep_net_client_core.h"
namespace small_server{
class RedisCore: public SheepNetClientCore{
public:
    RedisCore():
        SheepNetClientCore(SheepNetCore::GetInstance()->GetLoop()) {
        SetResolverType("string");
    }
};
};
