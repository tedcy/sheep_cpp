#include "small_timer.h"

#include <memory>
#include <boost/asio.hpp>

namespace small_timer{
class AsioTimer: public TimerI {
public:
    AsioTimer();
    ~AsioTimer() override;
    void AsyncWait(uint64_t ms, 
            std::function<void(const std::string& errMsg)>) override;
    void Cancel() override;
private:
    std::shared_ptr<
        boost::asio::deadline_timer> timer_ = nullptr;
};
}
