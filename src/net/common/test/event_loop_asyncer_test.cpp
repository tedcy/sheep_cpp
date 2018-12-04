#include "asyncer.h"
#include "log.h"

int main() {
    sheep::net::EventLoop loop;
    sheep::net::Asyncer asyncer(loop);
    asyncer.AsyncDo([&loop](const std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(WARNING) << errMsg;
            loop.Stop();
            return;
        }
        LOG(INFO) << "hello world";
        loop.Stop();
    });
    loop.Wait();
}
