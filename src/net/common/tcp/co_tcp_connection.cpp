#include "co_tcp_connection.h"
#include "coroutine_scheduler.h"
#include <memory>

using namespace std;

namespace sheep{
namespace net{

CoTcpConnection::CoTcpConnection(TcpConnection &connection)
    : connection_(&connection) {}

int CoTcpConnection::Write(string& errMsg, const char *buf, int len) {
    auto curCoro = CoroutineScheduler::currentCoroScheduler()->currentCoro();
    connection_->WriteBufferPush(buf, len);
    connection_->AsyncWrite([curCoro, &errMsg](const std::string &argErrMsg) {
        errMsg = argErrMsg;
        CoroutineScheduler::currentCoroScheduler()->resume(curCoro);
    });
    CoroutineScheduler::currentCoroScheduler()->suspend();
    return len;
}
int CoTcpConnection::Read(string& errMsg, char *buf, int len) {
    auto curCoro = CoroutineScheduler::currentCoroScheduler()->currentCoro();
    connection_->AsyncRead(len, [curCoro, &errMsg](const std::string &argErrMsg) {
        errMsg = argErrMsg;
        CoroutineScheduler::currentCoroScheduler()->resume(curCoro);
    });
    CoroutineScheduler::currentCoroScheduler()->suspend();
    if (!errMsg.empty()) {
        return 0;
    }
    connection_->ReadBufferPopHead(buf, len);
    return len;
}

void CoTcpConnection::Finish() {
    connection_->Finish("");
}

}
}