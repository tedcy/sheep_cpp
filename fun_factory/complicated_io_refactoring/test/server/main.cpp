#include "server.h"
#include "log.h"

int main(){
    std::string errMsg;
    
    EventLoop loop;

    Server server(loop, "127.0.0.1", 8888);
    server.SetConnectedHandler([](std::string &errMsg, TcpConnection &connection) {
        if(!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        LOG(INFO) << "connected";
        connection.AsyncRead(100, [](std::string &errMsg,
        TcpConnection &connection){
            LOG(INFO) << "readed";
            char buf[100];
            connection.ReadBuffer_.PopHead(buf, 100);
            connection.WriteBuffer_.Push(buf, 100);
            connection.AsyncWrite([](std::string &errMsg,
            TcpConnection &connection) {
                LOG(INFO) << "wrote";
                //connection.Finish(errMsg);
            });
        });
    });
    server.SetDisconnectedHandler([](std::string &errMsg) {
        if(!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
        LOG(INFO) << "disconnected";
    });
    server.Init(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
