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
    thread_->join();
}

TEST_F(TestAsyncer,Cancel) {
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.AsyncDo([this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    asyncer.Cancel();
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
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.AsyncDo([this](const std::string &errMsg) {
        if(!errMsg.empty()) {
            successed_ = true;
            loop_->Stop();
            return;
        }
        successed_ = false;
        loop_->Stop();
    });
    asyncer.Cancel();
    asyncer.Cancel();
    thread_->join();
}
TEST_F(TestAsyncer,CancelBeforeAsyncDo) {
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.Cancel();
    asyncer.AsyncDo([this](const std::string &errMsg) {
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
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.AsyncDo([this, &asyncer](const std::string &errMsg) {
        asyncer.Cancel();
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
    sheep::net::Asyncer asyncer(*loop_);
    asyncer.AsyncDo([this, &asyncer](const std::string &errMsg) {
    });
    asyncer.AsyncDo([this, &asyncer](const std::string &errMsg) {
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
