#pragma once
#include "sheep_net_core.h"
#include "sheep_net_client_core.h"
namespace small_server{
class RedisCore: public SheepNetClientCore{
public:
    static RedisCore* GetInstance() {
        static RedisCore redisCore(SheepNetCore::GetInstance()->GetLoop());
        redisCore.SetResolverType("string");
        return &redisCore;
    }
private:
    RedisCore(sheep::net::EventLoop &loop):
        SheepNetClientCore(loop) {
    }
};
};
