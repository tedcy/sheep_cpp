#include "net.h"
#include "log.h"

using namespace std;

int main(){
    std::string errMsg;
    
    sheep::net::EventLoop loop;

    sheep::net::Server server(loop, "127.0.0.1", 8888);
    server.SetConnectedHandler([](const std::string &errMsg, sheep::net::TcpConnection &connection) {
        if(!errMsg.empty()) {
            LOG(FATAL) << errMsg;
        }
        LOG(INFO) << "connected";
        connection.AsyncRead(100, [&connection](const std::string &errMsg){
            if (!errMsg.empty()) {
                LOG(ERROR) << errMsg << endl;
                connection.Finish(errMsg);
                return;
            }
            LOG(INFO) << "readed";
            char buf[100];
            connection.ReadBufferPopHead(buf, 100);
            connection.WriteBufferPush(buf, 100);
            connection.AsyncWrite([](const std::string &errMsg) {
                LOG(INFO) << "wrote";
                //connection.Finish(errMsg);
            });
        });
    });
    server.SetDisconnectedHandler([](const std::string &errMsg) {
        LOG(INFO) << "disconnected";
        if(!errMsg.empty()) {
            LOG(ERROR) << errMsg;
            return;
        }
    });
    server.Serve(errMsg);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    loop.Wait();
}
