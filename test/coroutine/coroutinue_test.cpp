#include "coroutine_info.h"
#include "small_test.h"
#include "coroutine_scheduler.h"

using namespace std;

class TestCoroutine: public testing::Test{
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