#pragma once
#include <functional>
#include <string>

namespace small_timer{
class TimerI {
public:
    TimerI() = default;
    virtual ~TimerI() = default;
    virtual void AsyncWait(uint64_t ms, 
            std::function<void(const std::string& errMsg)>) = 0;
    virtual void Cancel() = 0;
};
}//namespace small_timer
