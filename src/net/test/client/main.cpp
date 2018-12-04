#include "net.h"
#include "log.h"

int main(){
    std::string errMsg;
    
    sheep::net::EventLoop loop;

    sheep::net::Client client(loop, "127.0.0.1", 8888);
    client.SetConnectedHandler([](std::string &errMsg, sheep::net::TcpConnection &connection) {
        if(!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        LOG(INFO) << "connected";
        char buf[100];
        connection.WriteBuffer_.Push(buf, 100);
        connection.AsyncWrite([](std::string &errMsg,
        sheep::net::TcpConnection &connection) {
            LOG(INFO) << "wrote";
            connection.AsyncRead(100, [](std::string &errMsg,
            sheep::net::TcpConnection &connection){
                LOG(INFO) << "read";
                char buf[100];
                connection.ReadBuffer_.PopHead(buf, 100);
                connection.Finish(errMsg);
            });
        });
    });
    client.SetDisconnectedHandler([&loop](std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        LOG(INFO) << "disconnected";
        loop.Stop();
    });
    client.AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
