#include "net.h"
#include "log.h"

int main(){
    std::string errMsg;
    
    sheep::net::EventLoop loop;

    sheep::net::Server server(loop, "127.0.0.1", 8888);
    server.SetConnectedHandler([](const std::string &errMsg, sheep::net::TcpConnection &connection) {
        if(!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        LOG(INFO) << "connected";
        connection.AsyncRead(100, [&connection](std::string &errMsg){
            LOG(INFO) << "readed";
            char buf[100];
            connection.ReadBuffer_.PopHead(buf, 100);
            connection.WriteBuffer_.Push(buf, 100);
            connection.AsyncWrite([](std::string &errMsg) {
                LOG(INFO) << "wrote";
                //connection.Finish(errMsg);
            });
        });
    });
    server.SetDisconnectedHandler([](const std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        LOG(INFO) << "disconnected";
    });
    server.Serve(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
