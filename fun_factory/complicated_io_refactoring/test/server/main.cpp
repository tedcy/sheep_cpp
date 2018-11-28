#include "server.h"
#include "log.h"

int main(){
    std::string errMsg;
    
    EventLoop loop;

    Server server(loop, "127.0.0.1", 8888);
    server.Init(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    server.SetConnectedHandler([](std::string &errMsg) {
                if(!errMsg.empty()) {
                    LOG(FATAL) << errMsg;
                }
            });
    server.SetMessageHandler([&loop](std::string &errMsg,
                TcpConnection &connection){
                if(!errMsg.empty()) {
                    LOG(FATAL) << errMsg;
                }
                char buf[1024];
                auto count = connection.ReadBuffer_.PopHead(buf, 1024);
                LOG(INFO) << count;
                loop.Stop();
            });
    loop.Wait();
}
