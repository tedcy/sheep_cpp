#include "timer.h"
#include "log.h"

int main() {
    sheep::net::EventLoop loop;
    sheep::net::Timer timer(loop);
    timer.AsyncWait(5000, [&loop](const std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(WARNING) << errMsg;
            loop.Stop();
            return;
        }
        LOG(INFO) << "hello world";
        loop.Stop();
    });
    timer.Cancel();
    loop.Wait();
}
