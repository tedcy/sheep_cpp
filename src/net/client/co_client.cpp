#include "co_client.h"
#include "connector.h"
#include "coroutine_scheduler.h"
#include "log.h"

using namespace std;

namespace sheep{
namespace net{

CoClient::CoClient(const string &addr, int port)
    : client_(CoroutineScheduler::currentCoroScheduler()->getLoop(), addr,
              port) {}

CoTcpConnection CoClient::Connect(string &errMsg) {
    auto curCoro = CoroutineScheduler::currentCoroScheduler()->currentCoro();
    client_.SetConnectedHandler([curCoro](const string& errMsg) {
        CoroutineScheduler::currentCoroScheduler()->resume(curCoro);
    });
    client_.SetDisconnectedHandler([](const string& errMsg) {
    });
    client_.AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        return {};
    }
    CoroutineScheduler::currentCoroScheduler()->suspend();

    return CoTcpConnection(client_.GetTcpConnection());
}

}
}