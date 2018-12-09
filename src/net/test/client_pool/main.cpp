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
    for (int i = 0;i < 10;i++) {
        auto timer = std::make_shared<sheep::net::Timer>(loop);
        ts.push_back(timer);
        timer->AsyncWait(1000 * (i+1), [&clientPool](const std::string &errMsg){
            LOG(INFO) << "call SetConnectedHandler";
            auto client = clientPool.Get();
            if (!client) {
                LOG(WARNING) << "client nullptr";
                return;
            }
            auto &connection = client->GetTcpConnection();
            char buf[100];
            connection.WriteBuffer_.Push(buf, 100);
            connection.AsyncWrite([&clientPool, client](std::string &errMsg) {
                LOG(INFO) << "wrote";
                auto &connection = client->GetTcpConnection();
                connection.AsyncRead(100, [&clientPool, client](std::string &errMsg){
                    LOG(INFO) << "read";
                    char buf[100];
                    auto &connection = client->GetTcpConnection();
                    connection.ReadBuffer_.PopHead(buf, 100);
                    clientPool.Insert(client);
                    //connection.Finish(errMsg);
                });
            });
        });
    }
    loopThread.join();
}
