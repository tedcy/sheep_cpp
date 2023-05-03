#pragma once
#include <atomic>
#include <string>
#include "extends/small_packages/small_packages.h"
class TraceInstance: public small_packages::noncopyable{
public:
    static TraceInstance* Get() {
        static TraceInstance instance;
        return &instance;
    }
    std::string GetTraceId() {
        traceId_++;
        return std::to_string(traceId_);
    }
private:
    std::atomic<uint64_t> traceId_ = {0};
    TraceInstance() = default;
};
