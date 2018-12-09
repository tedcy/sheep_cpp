#include "net.h"
#include "log.h"

int main(){
    std::string errMsg;
    
    sheep::net::EventLoop loop;

    sheep::net::Client client(loop, "127.0.0.1", 8888);
    client.SetConnectedHandler([&client](const std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        LOG(INFO) << "connected";
        auto &connection = client.GetTcpConnection();
        char buf[100];
        connection.WriteBuffer_.Push(buf, 100);
        connection.AsyncWrite([&client](std::string &errMsg) {
            LOG(INFO) << "wrote";
            auto &connection = client.GetTcpConnection();
            connection.AsyncRead(100, [&client](std::string &errMsg){
                LOG(INFO) << "read";
                char buf[100];
                auto &connection = client.GetTcpConnection();
                connection.ReadBuffer_.PopHead(buf, 100);
                connection.Finish(errMsg);
            });
        });
    });
    client.SetDisconnectedHandler([&loop, &client](const std::string &argErrMsg) {
        LOG(INFO) << "disconnected";
        if(!argErrMsg.empty()) {
            LOG(ERROR) << argErrMsg;
        }
        std::string errMsg;
        client.AsyncConnect(errMsg);
        if (!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        //loop.Stop();
    });
    client.AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
