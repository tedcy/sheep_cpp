#pragma once
#include <atomic>
#include <string>
class TraceInstance{
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
    TraceInstance(const TraceInstance&) = default;
    TraceInstance& operator=(const TraceInstance&) = default;
};
