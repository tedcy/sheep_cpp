#pragma once
#include "timer.h"
#include "log.h"
#include "small_test.h"

class TestTimer: public testing::Test{
public:
    static void SetUpTestCase() {
    }
    static void TearDownTestCase() {
    }
    virtual void SetUp() {
        loop_ = new(sheep::net::EventLoop);
        successed_ = false;
    }
    virtual void TearDown() {
        EXPECT_TRUE(successed_);
        delete(loop_);
    }

    sheep::net::EventLoop *loop_;
    bool successed_;
};

TEST_F(TestTimer, Normal) {
    sheep::net::Timer timer(*loop_);
    timer.AsyncWait(50, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = false;
            loop_->Stop();
            return;
        }
        successed_ = true;
        loop_->Stop();
    });
    loop_->Wait();
}

TEST_F(TestTimer, Cancel) {
    sheep::net::Timer timer(*loop_);
    timer.AsyncWait(2000, [this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    timer.Cancel();
    loop_->Wait();
}
