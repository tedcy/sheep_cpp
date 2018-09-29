#include "glog.h"
#include <glog/logging.h>
#include <iostream>

namespace small_log{
GLogInfo::~GLogInfo() {
    std::cout << "~GLogInfo" << std::endl;
}
LogI& GLogDebug::operator<<(int64_t value) {
    LOG(INFO) << value;
    return *this;
}
LogI& GLogInfo::operator<<(int64_t value) {
    LOG(INFO) << value;
    return *this;
}
LogI& GLogWarning::operator<<(int64_t value) {
    LOG(WARNING) << value;
    return *this;
}
LogI& GLogError::operator<<(int64_t value) {
    LOG(ERROR) << value;
    return *this;
}
LogI& GLogFatal::operator<<(int64_t value) {
    LOG(FATAL) << value;
    return *this;
}

LogI& GLogDebug::operator<<(const std::string &value) {
    LOG(INFO) << value;
    return *this;
}
LogI& GLogInfo::operator<<(const std::string &value) {
    LOG(INFO) << value;
    return *this;
}
LogI& GLogWarning::operator<<(const std::string &value) {
    LOG(WARNING) << value;
    return *this;
}
LogI& GLogError::operator<<(const std::string &value) {
    LOG(ERROR) << value;
    return *this;
}
LogI& GLogFatal::operator<<(const std::string &value) {
    LOG(FATAL) << value;
    return *this;
}
}
