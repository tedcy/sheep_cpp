#include "glog.h"
#include <iostream>

namespace small_log{
//when glog global value destoryed
//call LOG cause panic
//class static value destoryed before glog's global values
//it's ok to test if glog destoryed

GLog::~GLog() {
    //std::cout << "~GLog" << std::endl;
}

extern bool gDestoryed;

GLog::GLog(Level l, const char* file, uint32_t line) {
    //std::cout << "GLog if destoryed " << gDestoryed << std::endl;
    if (gDestoryed) {
        //std::cout << "GLog destoryed" << std::endl;
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
        //std::cout << "GLog no destoryed" << std::endl;
        message_ = std::unique_ptr<google::LogMessage>(
                new google::LogMessage(file, line, google::GLOG_INFO));
        return;
    }
}
std::ostream& GLog::stream() {
    //std::cout << "GLog stream" << std::endl;
    if (stream_ != nullptr) {
        //std::cout << "GLog stream destoryed" << std::endl;
        return *stream_.get();
    }
    return message_->stream();
}
}
