#pragma once
#include <unistd.h>
#include "event_loop.h"
#include "client.h"
#include "log.h"
#include "small_test.h"
#include <thread>

class TestClient: public testing::Test{
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

TEST_F(TestClient, Connected) {
    sheep::net::Client client(*loop_, "127.0.0.1", 22);
    client.SetConnectedHandler([this](const std::string &errMsg){
        loop_->Stop();
        successed_ = true;
    });
    client.SetDisconnectedHandler([this](const std::string &errMsg){
        loop_->Stop();
        successed_ = false;
    });
    std::string errMsg;
    client.AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
    }
    thread_->join();
}

TEST_F(TestClient, ConnectFailed) {
    sheep::net::Client client(*loop_, "127.0.0.1", 1);
    client.SetConnectedHandler([this](const std::string &errMsg){
        loop_->Stop();
        successed_ = false;
    });
    client.SetDisconnectedHandler([this](const std::string &errMsg){
        loop_->Stop();
        successed_ = true;
    });
    std::string errMsg;
    client.AsyncConnect(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = true;
    }
    thread_->join();
}
