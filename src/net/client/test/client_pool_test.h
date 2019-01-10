#pragma once
#include <unistd.h>
#include "event_loop.h"
#include "client_pool.h"
#include "log.h"
#include "small_test.h"
#include "timer.h"
#include "block_server_for_test.h"
#include <thread>

class TestClientPool: public testing::Test{
public:
    TestClientPool() :
        server_(block_server::ServerThread::GetInstance().GetServer()),
        port_(server_.GetPort()) {
    }
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
        server_.Close();
        delete(thread_);
        delete(loop_);
        EXPECT_TRUE(successed_);
    }

    block_server::Server &server_;
    uint32_t port_;
    sheep::net::EventLoop *loop_;
    std::thread *thread_;
    bool successed_;
};

TEST_F(TestClientPool, 1B23_SheepThreadClosedBeforeClientAsyncWrite) {
    std::string errMsg;
    auto clientPool = std::make_shared<sheep::net::ClientPool>(*loop_, "127.0.0.1", port_, 1);
    clientPool->Init(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto client = clientPool->Get();
    auto &connection = client->GetTcpConnection();
    server_.Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    connection.AsyncWrite([this, &clientPool, &client](const std::string &errMsg){
        if (!errMsg.empty()) {
            clientPool->Insert(client, errMsg);
            successed_ = true;
            loop_->Stop();
        }
    });
    thread_->join();
}

TEST_F(TestClientPool, 12A3_12A4_MainThreadClosedAfterClientAsyncWrite) {
    std::string errMsg;
    auto clientPool = std::make_shared<sheep::net::ClientPool>(*loop_, "127.0.0.1", port_, 1);
    clientPool->Init(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto client = clientPool->Get();
    auto &connection = client->GetTcpConnection();
    connection.AsyncWrite([this, &clientPool, &client](const std::string &errMsg){
        if (!errMsg.empty()) {
            clientPool->Insert(client, errMsg);
            successed_ = true;
            loop_->Stop();
        }
    });
    clientPool->Insert(client, "testFinish");
    thread_->join();
}

TEST_F(TestClientPool, 12B3_12B4_SheepThreadClosedAfterClientAsyncWrite) {
    std::string errMsg;
    auto clientPool = std::make_shared<sheep::net::ClientPool>(*loop_, "127.0.0.1", port_, 1);
    clientPool->Init(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto client = clientPool->Get();
    auto &connection = client->GetTcpConnection();
    uint64_t bigLen = 30*1024*1024;
    auto buf = new char[bigLen];
    connection.WriteBufferPush(buf, bigLen);
    delete []buf;
    connection.AsyncWrite([this, &clientPool, &client](const std::string &errMsg){
        if (!errMsg.empty()) {
            clientPool->Insert(client, errMsg);
            successed_ = true;
            loop_->Stop();
        }
    });
    server_.Close();
    thread_->join();
}

TEST_F(TestClientPool, 1245B6_SheepClosedAfterClientAsyncReadAny) {
    std::string errMsg;
    auto clientPool = std::make_shared<sheep::net::ClientPool>(*loop_, "127.0.0.1", port_, 1);
    clientPool->Init(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto client = clientPool->Get();
    auto &connection = client->GetTcpConnection();
    char buf[1024];
    connection.WriteBufferPush(buf, 1024);
    connection.AsyncWrite([this, &clientPool, &client](const std::string &errMsg){
        if (!errMsg.empty()) {
            clientPool->Insert(client, errMsg);
            successed_ = false;
            loop_->Stop();
        }
        auto &connection = client->GetTcpConnection();
        connection.AsyncReadAny([this, &clientPool, &client](const std::string &errMsg) {
            if (!errMsg.empty()) {
                clientPool->Insert(client, errMsg);
                successed_ = true;
                loop_->Stop();
            }
        });
        server_.Close();
    });
    thread_->join();
}

TEST_F(TestClientPool, 12456B_SheepClosedInAsyncReadAnyHandler) {
    std::string errMsg;
    auto clientPool = std::make_shared<sheep::net::ClientPool>(*loop_, "127.0.0.1", port_, 1);
    clientPool->Init(errMsg);
    if (!errMsg.empty()) {
        loop_->Stop();
        successed_ = false;
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto client = clientPool->Get();
    auto &connection = client->GetTcpConnection();
    char buf[1024];
    connection.WriteBufferPush(buf, 1024);
    connection.AsyncWrite([this, &clientPool, &client](const std::string &errMsg){
        if (!errMsg.empty()) {
            clientPool->Insert(client, errMsg);
            successed_ = false;
            loop_->Stop();
        }
        auto &connection = client->GetTcpConnection();
        connection.AsyncReadAny([this, &clientPool, &client](const std::string &errMsg) {
            if (!errMsg.empty()) {
                clientPool->Insert(client, errMsg);
                successed_ = false;
                loop_->Stop();
            }
            clientPool->Insert(client, "testFinish");
            successed_ = true;
            loop_->Stop();
        });
        char buf[1024];
        server_.Write(buf, 1024);
    });
    thread_->join();
}
