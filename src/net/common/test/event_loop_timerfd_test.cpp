#include <sys/timerfd.h>
#include <unistd.h>
#include "event_loop.h"
#include "event.h"
#include "log.h"
#include "epoller.h"

int main() {
    sheep::net::EventLoop loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    auto event = std::make_shared<sheep::net::Event>(loop, 
            sheep::net::EpollerFactory::Get()->GetPollerType(), timerfd);
    event->SetReadEvent([&loop, event](){
                LOG(INFO) << "hello world";
                event->DisableReadNotify();
                loop.Stop();
            });
    event->EnableReadNotify();

    struct itimerspec howlong = {0};
    howlong.it_value.tv_sec = 1;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.Wait();
    ::close(timerfd);
}
