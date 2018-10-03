#include "log.h"

#include <iostream>

struct A{
    ~A() {
        std::cout << "~A" << std::endl;
        LOG(INFO) << "fix glog core bug";
    }
};
A a;

int main() {
    //small_log::Init("log", "main");
    small_log::EnableTrace();
    LOG(INFO) << "info";
}
