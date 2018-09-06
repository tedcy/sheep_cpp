#include "small_timer.h"

namespace small_timer{
class AsioTimer: public TimerI {
public:
    AsioTimer();
    ~AsioTimer() override;
    void AsyncWait(uint64_t ms, 
            std::function<void(const std::string& errMsg)>) override;
};
}
