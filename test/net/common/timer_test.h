#pragma once
#include "timer.h"
#include "log.h"
#include "small_test.h"
#include <thread>

const int g_shortTime = 1;
const int g_normalTime = 10;
const int g_longTime = 1000;

class TestTimer: public testing::Test{
public:
    static void SetUpTestCase() {
    }
    static void TearDownTestCase() {
    }
    virtual void SetUp() {
        loop_ = new(sheep::net::EventLoop);
        successed_ = false;
        thread_ = new std::thread([this](){
            loop_->Wait();
        });
    }
    virtual void TearDown() {
        delete(thread_);
        delete(loop_);
        EXPECT_TRUE(successed_);
    }

    sheep::net::EventLoop *loop_;
    std::thread *thread_;
    bool successed_;
};

TEST_F(TestTimer, Normal) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_normalTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = false;
            loop_->Stop();
            return;
        }
        successed_ = true;
        loop_->Stop();
    });
    thread_->join();
}

TEST_F(TestTimer, Cancel) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_longTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    timer->Cancel();
    thread_->join();
}

TEST_F(TestTimer, CancelWaitNormalTime) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_longTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(g_normalTime));
    timer->Cancel();
    thread_->join();
}

TEST_F(TestTimer, CancelNullptr) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_longTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    timer = nullptr;
    thread_->join();
}

TEST_F(TestTimer, CancelNullptrWaitNormalTime) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_longTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(g_normalTime));
    timer = nullptr;
    thread_->join();
}

TEST_F(TestTimer, CancelTwice) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_longTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    timer->Cancel();
    timer->Cancel();
    thread_->join();
}

TEST_F(TestTimer, CancelBeforeAsyncWait) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->Cancel();
    timer->AsyncWait(g_normalTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = false;
            loop_->Stop();
            return;
        }
        successed_ = true;
        loop_->Stop();
    });
    thread_->join();
}

TEST_F(TestTimer, CancelInAsyncWait) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_normalTime, [this, &timer](const std::string &errMsg) {
        timer->Cancel();
        if(!errMsg.empty()) {
            successed_ = false;
            loop_->Stop();
            return;
        }
        successed_ = true;
        loop_->Stop();
    });
    thread_->join();
}

TEST_F(TestTimer, Reuse) {
    auto timer = std::make_shared<sheep::net::Timer>(*loop_);
    timer->AsyncWait(g_shortTime, [this](const std::string &errMsg) {
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(g_normalTime));
    timer->AsyncWait(g_normalTime, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    thread_->join();
}

TEST_F(TestTimer, Threads) {
    std::vector<std::shared_ptr<std::thread>> threads;
    std::atomic<int> count = {0};
    int threads_count = 10;
    int job_nums = 10000;
    for (int i = 0;i < threads_count;i++) {
        auto thread = std::make_shared<std::thread>([this, &count, job_nums](){
            for (int i = 0;i < job_nums;i++) {
                auto timer = std::make_shared<sheep::net::Timer>(*loop_);
                timer->AsyncWait(g_shortTime, [this, &count](const std::string &errMsg) {
                    count++;
                });
            }
        });
        threads.push_back(thread);
    }
    for (int i = 0;i < threads_count;i++) {
        threads[i]->join();
    }
    if (count == threads_count * job_nums) {
        successed_ = true;
    }
    EXPECT_EQ(count, threads_count * job_nums);
    loop_->Stop();
    thread_->join();
}
