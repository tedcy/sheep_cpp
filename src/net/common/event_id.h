#pragma once

#include <atomic>
#include "small_packages.h"

namespace sheep{
namespace net{
class EventIdCreator: public small_packages::noncopyable {
public:
    static EventIdCreator* Get() {
        static EventIdCreator idCreator;
        return &idCreator;
    }
    int64_t Create() {
        return current++;
    }
private:
    EventIdCreator() = default;
    std::atomic<int64_t> current = {0};
};
}
}
