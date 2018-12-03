#include <unistd.h>
#include "event_loop.h"
#include "event.h"
#include "log.h"
#include "epoller.h"

int main() {
    EventLoop loop;
    auto event = std::make_shared<Event>(loop, EpollerFactory::Get()->GetPollerType(), STDIN_FILENO);
    event->SetReadEvent([&loop, event](){
                char buf[1024] = {0};
                auto result = read(STDIN_FILENO, buf, sizeof(buf));
                LOG(INFO) << "hello world";
                //event->DisableReadNotify();
                //loop.Stop();
            });
    event->EnableReadNotify();

    loop.Wait();
}
