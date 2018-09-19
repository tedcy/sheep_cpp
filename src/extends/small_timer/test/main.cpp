#include "small_timer_factory.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>
#include <list>

int main() {
    //small_log::Init();
    small_timer::Init();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG(INFO) << "start";
    
    std::list<std::shared_ptr<small_timer::TimerI>> ts;
    for(int i = 0;i < 100;i++) {
        auto t = small_timer::MakeTimer();
        t->AsyncWait(10, [](const std::string &errMsg) {
                if (errMsg != "") {
                    LOG(ERROR) << errMsg;
                    return;
                }
                LOG(INFO) << "ok";
            });
        //t->Cancel();
        ts.emplace_back(t);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1000));
}
