#pragma once
#include "asyncer.h"
#include "log.h"
#include "small_test.h"

class TestAsyncer: public testing::Test{
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

TEST_F(TestAsyncer,Normal) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this](const std::string &errMsg) {
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

TEST_F(TestAsyncer,Cancel) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    asyncer->Cancel();
    thread_->join();
}

TEST_F(TestAsyncer,SetNullptrCancel) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    asyncer = nullptr;
    thread_->join();
}

TEST_F(TestAsyncer,CancelTwice) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    asyncer->Cancel();
    asyncer->Cancel();
    thread_->join();
}
TEST_F(TestAsyncer,CancelBeforeAsyncDo) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->Cancel();
    asyncer->AsyncDo([this](const std::string &errMsg) {
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
TEST_F(TestAsyncer,CancelInAsyncDo) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this, &asyncer](const std::string &errMsg) {
        asyncer->Cancel();
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
TEST_F(TestAsyncer, Reuse) {
    auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
    asyncer->AsyncDo([this, &asyncer](const std::string &errMsg) {
    });
    asyncer->AsyncDo([this, &asyncer](const std::string &errMsg) {
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

TEST_F(TestAsyncer, Threads) {
    std::vector<std::shared_ptr<std::thread>> threads;
    std::atomic<int> count = {0};
    int threads_count = 10;
    int job_nums = 10000;
    for (int i = 0;i < threads_count;i++) {
        auto thread = std::make_shared<std::thread>([this, &count, job_nums](){
            for (int i = 0;i < job_nums;i++) {
                auto asyncer = std::make_shared<sheep::net::Asyncer>(*loop_);
                asyncer->AsyncDo([this, &count](const std::string &errMsg) {
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
