#include "coroutine_info.h"
#include "small_test.h"
#include "coroutine_scheduler.h"
#include "net.h"
#include "log.h"

using namespace std;

class TestCoroutine: public testing::Test{
    bool isDown = false;
    mutex mtx_;
    condition_variable cv_;
public:
    void setDone() {
        unique_lock<mutex> lock(mtx_);
        isDown = true;
        cv_.notify_one();
    }
    void waitDone() {
        unique_lock<mutex> lock(mtx_);
        while(!isDown) {
            cv_.wait(lock);
        }
    }
};

TEST_F(TestCoroutine, TestCoroutineInfo) {
    CoroutineInfo info;
    auto runFunc = []() {
        cout << "runFunc" << endl;
    };
    auto endFunc = []() {
        cout << "endFunc" << endl;
    };
    info.registerFunc(runFunc, endFunc);

    CoroutineInfo::switchTo(&info);
}

TEST_F(TestCoroutine, TestCoroutineScheduler) {
    CoroutineScheduler scheduler;
    scheduler.start();

    scheduler.addCoroutine([](){
        cout << 1 << endl;
        CoroutineScheduler::currentCoroScheduler()->yield();
        cout << 2 << endl;
        CoroutineScheduler::currentCoroScheduler()->yield();
        cout << 3 << endl;
    });

    scheduler.stop();
}

TEST_F(TestCoroutine, TestCoroutineSuspend) {
    CoroutineScheduler scheduler;
    scheduler.start();

    CoroutineInfo *info;
    scheduler.addCoroutine([&info](){
        info = CoroutineScheduler::currentCoroScheduler()->currentCoro();

        LOG(INFO) << "suspend" << endl;
        CoroutineScheduler::currentCoroScheduler()->suspend();
        LOG(INFO) << "suspend end" << endl;
    });
    sleep(1);
    LOG(INFO) << "resume" << endl;
    scheduler.resume(info);

    scheduler.stop();
}

TEST_F(TestCoroutine, TestCoClient) {
    sheep::net::EventLoop loop;
    CoroutineScheduler scheduler(loop);
    
    scheduler.start();
    scheduler.addCoroutine([this]() {
        shared_ptr<int> defer(nullptr, [this](int *) { setDone(); });
        string errMsg;
        sheep::net::CoClient client("127.0.0.1", 8888);
        auto connection = client.Connect(errMsg);
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg << endl;
            return;
        }
        LOG(INFO) << "connected" << endl;
        char buf[100];
        connection.Write(errMsg, buf, 100);
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg << endl;
            return;
        }
        LOG(INFO) << "wrote" << endl;
        connection.Read(errMsg, buf, 100);
        if (!errMsg.empty()) {
            LOG(ERROR) << errMsg << endl;
            return;
        }
        LOG(INFO) << "read" << endl;
        connection.Finish();
    });
    waitDone();
    scheduler.stop();
}

TEST_F(TestCoroutine, TestCoServer) {
    sheep::net::EventLoop loop;
    CoroutineScheduler scheduler(loop);
    
    scheduler.start();

    sheep::net::CoServer server(
        scheduler, "127.0.0.1", 8888,
        [this](const std::string &errMsg,
               std::shared_ptr<sheep::net::CoTcpConnection> connection) {
            shared_ptr<int> defer(nullptr, [this](int *) { setDone(); });
            if (!errMsg.empty()) {
                LOG(ERROR) << errMsg << endl;
                return;
            }
            LOG(INFO) << "connected" << endl;
            {
                string errMsg;
                char buf[100];
                connection->Read(errMsg, buf, 100);
                if (!errMsg.empty()) {
                    LOG(ERROR) << errMsg << endl;
                    connection->Finish();
                    return;
                }
                LOG(INFO) << "read" << endl;
                connection->Write(errMsg, buf, 100);
                if (!errMsg.empty()) {
                    LOG(ERROR) << errMsg << endl;
                    connection->Finish();
                    return;
                }
                LOG(INFO) << "wrote" << endl;
                connection->Read(errMsg, buf, 1);
                if (!errMsg.empty()) {
                    LOG(ERROR) << errMsg << endl;
                    connection->Finish();
                    return;
                }
            }
        });
    string errMsg;
    server.Serve(errMsg);
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg << endl;
        return;
    }
    waitDone();
    scheduler.stop();
}
