#pragma once
#include <unistd.h>
#include "event_loop.h"
#include "event.h"
#include "log.h"
#include "epoller.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include "small_test.h"

class TestEvent: public testing::Test{
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

/*TEST_F(TestEvent, Normal) {
    auto event = std::make_shared<sheep::net::Event>(*loop_, sheep::net::EpollerFactory::Get()->GetPollerType(), STDIN_FILENO);
    event->SetReadEvent([event, this](){
        char buf[1024] = {0};
        auto result = read(STDIN_FILENO, buf, sizeof(buf));
        successed_ = true;
        event->DisableReadNotify();
        loop_->Stop();
    });
    event->EnableReadNotify();
}*/

TEST_F(TestEvent, TimerFd) {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    auto event = std::make_shared<sheep::net::Event>(*loop_, 
            sheep::net::EpollerFactory::Get()->GetPollerType(), timerfd);
    event->SetReadEvent([event, this](){
                successed_ = true;
                event->DisableReadNotify();
                loop_->Stop();
            });
    event->EnableReadNotify();

    struct itimerspec howlong = {0};
    //50ms
    howlong.it_value.tv_nsec = 50000000;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    thread_->join();
    ::close(timerfd);
}
