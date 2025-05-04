#include "coroutine_info.h"
#include "small_test.h"
#include "coroutine_scheduler.h"
#include "net.h"
#include "log.h"
#include "coroutine_mutex.h"

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

class TestMutex: public testing::Test{
    mutex mtx_;
    condition_variable cv_;
    vector<bool> needDones_;
public:
    void initDone(int num) {
        needDones_.resize(num);
    }
    void setDone() {
        unique_lock<mutex> lock(mtx_);
        needDones_.pop_back();
        cv_.notify_one();
    }
    void waitDone() {
        unique_lock<mutex> lock(mtx_);
        while(!needDones_.empty()) {
            cv_.wait(lock);
        }
    }
};

TEST_F(TestMutex, TestMutexInThread) {
    Mutex my_mutex;
    int counter = 0;
    const int kNumThreads = 10;
    const int kIncrementsPerThread = 100000;

    auto increment_func = [&]() {
        for (int i = 0; i < kIncrementsPerThread; ++i) {
            my_mutex.lock();
            ++counter;
            my_mutex.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kNumThreads; ++i) {
        threads.emplace_back(increment_func);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter, kNumThreads * kIncrementsPerThread);
}

TEST_F(TestMutex, TestMutexInThreadAndCoroutine) {
    sheep::net::EventLoop loop;
    CoroutineScheduler scheduler1(loop);
    CoroutineScheduler scheduler2(loop);
    
    scheduler1.start();
    scheduler2.start();
    Mutex my_mutex;
    int counter = 0;
    const int kNumThreads = 10;
    const int kIncrementsPerThread = 100000;

    initDone(10);

    for (int i = 0; i < kNumThreads / 2; ++i) {
        scheduler1.addCoroutine([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                my_mutex.lock();
                ++counter;
                my_mutex.unlock();
            }
            setDone();
        });
    }
    for (int i = 0; i < kNumThreads / 2; ++i) {
        scheduler2.addCoroutine([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                my_mutex.lock();
                ++counter;
                my_mutex.unlock();
            }
            setDone();
        });
    }

    waitDone();
    scheduler1.stop();
    scheduler2.stop();

    EXPECT_EQ(counter, kNumThreads * kIncrementsPerThread);
}

TEST_F(TestMutex, TestMutexInCoroutine) {
    sheep::net::EventLoop loop;
    CoroutineScheduler scheduler(loop);
    
    scheduler.start();

    Mutex my_mutex;
    int counter = 0;
    const int kNumThreads = 10;
    const int kIncrementsPerThread = 100000;

    initDone(kNumThreads / 2);

    for (int i = 0; i < kNumThreads / 2; ++i) {
        scheduler.addCoroutine([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                my_mutex.lock();
                ++counter;
                my_mutex.unlock();
            }
            setDone();
        });
    }

    auto increment_func = [&]() {
        for (int i = 0; i < kIncrementsPerThread; ++i) {
            my_mutex.lock();
            ++counter;
            my_mutex.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kNumThreads / 2; ++i) {
        threads.emplace_back(increment_func);
    }

    for (auto& t : threads) {
        t.join();
    }
    
    waitDone();
    scheduler.stop();

    EXPECT_EQ(counter, kNumThreads * kIncrementsPerThread);
}

TEST_F(TestMutex, TestCoMutexInCoroutine) {
    sheep::net::EventLoop loop;
    CoroutineScheduler scheduler1(loop);
    CoroutineScheduler scheduler2(loop);
    
    scheduler1.start();
    scheduler2.start();

    CoroutineMutex my_mutex;
    int counter = 0;
    const int kNumThreads = 10;
    const int kIncrementsPerThread = 100000;

    initDone(kNumThreads);

    for (int i = 0; i < kNumThreads / 2; ++i) {
        scheduler1.addCoroutine([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                my_mutex.lock();
                ++counter;
                my_mutex.unlock();
            }
            setDone();
        });
    }
    for (int i = 0; i < kNumThreads / 2; ++i) {
        scheduler2.addCoroutine([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                my_mutex.lock();
                ++counter;
                my_mutex.unlock();
            }
            setDone();
        });
    }

    waitDone();
    scheduler1.stop();
    scheduler2.stop();

    EXPECT_EQ(counter, kNumThreads * kIncrementsPerThread);
}