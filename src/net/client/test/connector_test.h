#pragma once
#include <unistd.h>
#include "event_loop.h"
#include "connector.h"
#include "log.h"
#include "small_test.h"
#include <thread>

class TestConnector: public testing::Test{
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

TEST_F(TestConnector, Connected) {
    sheep::net::Connector connector(*loop_, "127.0.0.1", 22);
    connector.SetNewConnectionHandler([this](std::unique_ptr<sheep::net::Socket> &socket, 
                std::shared_ptr<sheep::net::Event> event){
        successed_ = true;
        loop_->Stop();
    });
    connector.SetConnectFailedHandler([this](const std::string &errMsg) {
        successed_ = false;
        loop_->Stop();
    });
    std::string errMsg;
    connector.Connect(errMsg);
    if (!errMsg.empty()) {
        successed_ = false;
        loop_->Stop();
    }
    thread_->join();
}

TEST_F(TestConnector, ConnectFailed) {
    sheep::net::Connector connector(*loop_, "127.0.0.1", 1);
    connector.SetNewConnectionHandler([this](std::unique_ptr<sheep::net::Socket> &socket, 
                std::shared_ptr<sheep::net::Event> event){
        successed_ = false;
        loop_->Stop();
    });
    connector.SetConnectFailedHandler([this](const std::string &errMsg) {
        successed_ = true;
        loop_->Stop();
    });
    std::string errMsg;
    connector.Connect(errMsg);
    if (!errMsg.empty()) {
        successed_ = true;
        loop_->Stop();
    }
    thread_->join();
}
