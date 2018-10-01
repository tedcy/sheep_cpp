#include "glog.h"
#include <iostream>

namespace small_log{
//when glog global value destoryed
//call LOG cause panic
//class static value destoryed before glog's global values
//it's ok to test if glog destoryed

GLog::GLog(Level l, const char* file, uint32_t line, bool destoryed) {
    if (destoryed) {
        stream_ = std::unique_ptr<std::ostream>
            (new std::ostream(std::cout.rdbuf()));
        return;
    }
    switch (l) {
    case DEBUG:
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_INFO));
        return;
    case WARNING:
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_WARNING));
        return;
    case ERROR:
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_ERROR));
        return;
    case FATAL:
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_FATAL));
        return;
    case INFO:
    default:
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_INFO));
        return;
    }
}
std::ostream& GLog::stream() {
    if (stream_ != nullptr) {
        return *stream_.get();
    }
    return message_->stream();
}
}
