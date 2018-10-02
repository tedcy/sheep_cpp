#include "small_timer_factory.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>
#include <list>

#include "small_net.h"

using namespace small_log;

int main() {
    //small_log::Init();
    small_net::AsioNet::GetInstance().Init();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG(INFO) << "start";
    
    std::list<std::shared_ptr<small_timer::TimerI>> ts;
    for(int i = 0;i < 10;i++) {
        auto t = small_timer::MakeTimer();
        t->AsyncWait(1000, [](const std::string &errMsg) {
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
    small_net::AsioNet::GetInstance().Shutdown();
}
