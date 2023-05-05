#include "acceptor.h"
#include "event_loop.h"
#include "event.h"
#include "socket.h"
#include "log.h"

int main() {
    sheep::net::EventLoop loop;
    sheep::net::Acceptor acceptor(loop, "127.0.0.1", 8888);
    
    std::string errMsg;
    acceptor.SetNewConnectionHandler([&loop](int fd){
                LOG(INFO) << "accept: " << fd;
                loop.Stop();
            });
    acceptor.Listen(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
