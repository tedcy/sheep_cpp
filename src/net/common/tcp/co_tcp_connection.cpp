#include "co_tcp_connection.h"
#include "coroutine_scheduler.h"
#include <memory>

using namespace std;

namespace sheep{
namespace net{

CoTcpConnection::CoTcpConnection(TcpConnection &connection)
    : connection_(&connection) {}

CoTcpConnection::CoTcpConnection(std::shared_ptr<TcpConnection> connection)
    : connection_(connection.get()), connection_sptr_(connection) {}

int CoTcpConnection::Write(string& errMsg, const char *buf, int len) {
    auto curCoro = CoroutineScheduler::currentCoroScheduler()->currentCoro();
    TcpConnection* conn = connection_sptr_ ? connection_sptr_.get() : connection_;
    conn->WriteBufferPush(buf, len);
    conn->AsyncWrite([curCoro, &errMsg](const std::string &argErrMsg) {
        errMsg = argErrMsg;
        CoroutineScheduler::currentCoroScheduler()->resume(curCoro);
    });
    CoroutineScheduler::currentCoroScheduler()->suspend();
    return len;
}
int CoTcpConnection::Read(string& errMsg, char *buf, int len) {
    auto curCoro = CoroutineScheduler::currentCoroScheduler()->currentCoro();
    TcpConnection* conn = connection_sptr_ ? connection_sptr_.get() : connection_;
    conn->AsyncRead(len, [curCoro, &errMsg](const std::string &argErrMsg) {
        errMsg = argErrMsg;
        CoroutineScheduler::currentCoroScheduler()->resume(curCoro);
    });
    CoroutineScheduler::currentCoroScheduler()->suspend();
    if (!errMsg.empty()) {
        return 0;
    }
    conn->ReadBufferPopHead(buf, len);
    return len;
}

void CoTcpConnection::Finish() {
    TcpConnection* conn = connection_sptr_ ? connection_sptr_.get() : connection_;
    conn->Finish("");
}

}
}
