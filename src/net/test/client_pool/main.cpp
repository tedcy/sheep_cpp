#include "net.h"
#include "log.h"
#include <thread>

int main(){
    std::string errMsg;
    
    sheep::net::EventLoop loop;

    std::thread loopThread([&loop](){
        loop.Wait();
    });

    sheep::net::ClientPool clientPool(loop, "127.0.0.1", 8888, 1);
    clientPool.Init(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    std::vector<std::shared_ptr<sheep::net::Timer>> ts;
    //FIXME something bug with timer twice or more called together
    for (int i = 0;i < 1;i++) {
        auto timer = std::make_shared<sheep::net::Timer>(loop);
        ts.push_back(timer);
        timer->AsyncWait(1000 * i, [&clientPool](const std::string &errMsg){
            LOG(INFO) << "call SetConnectedHandler";
            auto client = clientPool.Get();
            if (!client) {
                LOG(WARNING) << "client nullptr";
                return;
            }
            auto &connection = client->GetTcpConnection();
            char buf[100];
            connection.WriteBufferPush(buf, 100);
            connection.AsyncWrite([&clientPool, client](const std::string &errMsg) {
                LOG(INFO) << "wrote";
                auto &connection = client->GetTcpConnection();
                connection.AsyncRead(100, [&clientPool, client](const std::string &errMsg){
                    LOG(INFO) << "read";
                    char buf[100];
                    auto &connection = client->GetTcpConnection();
                    connection.ReadBufferPopHead(buf, 100);
                    //test pushback to clientPool
                    //clientPool.Insert(client);
                    //test reconnect
                    connection.Finish("shutdown");
                });
            });
        });
    }
    loopThread.join();
}
