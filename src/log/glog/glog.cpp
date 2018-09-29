#include "glog.h"
#include <glog/logging.h>
#include <iostream>

namespace small_log{
//when glog global value destoryed
//call LOG cause panic
//class static value destoryed before glog's global values
//it's ok to test if glog destoryed
bool gLogDestoryed = false;
GLogInfo::~GLogInfo() {
    gLogDestoryed = true;
}
LogI& GLogDebug::operator<<(int64_t value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(INFO) << value;
    return *this;
}
LogI& GLogInfo::operator<<(int64_t value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(INFO) << value;
    return *this;
}
LogI& GLogWarning::operator<<(int64_t value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(WARNING) << value;
    return *this;
}
LogI& GLogError::operator<<(int64_t value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(ERROR) << value;
    return *this;
}
LogI& GLogFatal::operator<<(int64_t value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(FATAL) << value;
    return *this;
}

LogI& GLogDebug::operator<<(const std::string &value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(INFO) << value;
    return *this;
}
LogI& GLogInfo::operator<<(const std::string &value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(INFO) << value;
    return *this;
}
LogI& GLogWarning::operator<<(const std::string &value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(WARNING) << value;
    return *this;
}
LogI& GLogError::operator<<(const std::string &value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(ERROR) << value;
    return *this;
}
LogI& GLogFatal::operator<<(const std::string &value) {
    if (gLogDestoryed) {
        std::cout << value << std::endl;
        return *this;
    }
    LOG(FATAL) << value;
    return *this;
}
}
