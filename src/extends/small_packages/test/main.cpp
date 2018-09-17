#include "small_packages.h"
#include "log.h"

int main() {
    std::string str{"1,2,3,4,5,6"};
    auto ss = small_strings::split(str, ',');
    for(auto &s: *ss) {
        LOG(INFO) << s;
    }
}
