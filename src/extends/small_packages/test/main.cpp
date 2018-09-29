#include "small_packages.h"
#include "log.h"

#include <thread>

using namespace small_log;

void test_lock() {
    auto lock = small_lock::MakeLock();
    int i = 0;

    std::thread thread1([lock, &i](){
        int count = 0;
        for (;i < 10000000;) {
            small_lock::UniqueGuard unique_lock(lock);
            count++;
            i++;
            //LOG(INFO) << i;
        }
        LOG(INFO) << count;
    });
    std::thread thread2([lock, &i](){
        int count = 0;
        for (;i < 10000000;) {
            small_lock::UniqueGuard unique_lock(lock);
            count++;
            i++;
            //LOG(INFO) << i;
        }
        LOG(INFO) << count;
    });
    thread1.join();
    thread2.join();
}

void test_strings() {
    std::string str{"1,2,3,4,5,6"};
    auto ss = small_strings::split(str, ',');
    for(auto &s: *ss) {
        LOG(INFO) << s;
    }
}

int main() {
    test_strings();
    test_lock();
}
