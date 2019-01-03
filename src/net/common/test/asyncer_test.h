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
    }
    virtual void TearDown() {
        EXPECT_TRUE(successed_);
        delete(loop_);
    }

    sheep::net::EventLoop *loop_;
    bool successed_;
};

TEST_F(TestAsyncer,Normal) {
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.AsyncDo([this](const std::string &errMsg) {
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
    loop_->Wait();
}
