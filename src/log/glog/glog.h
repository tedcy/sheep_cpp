#pragma once
#include "log.h"
#include "log_interface.h"
#include <string>

namespace small_log{
class GLogDebug: public LogI{
public:
    static LogI& GetInstance() {
        static GLogDebug instance_;
        return instance_;
    };
    LogI& operator<<(int64_t value);
    LogI& operator<<(const std::string &value);
};
class GLogInfo: public LogI{
public:
    ~GLogInfo();
    static LogI& GetInstance() {
        static GLogInfo instance_;
        return instance_;
    };
    LogI& operator<<(int64_t value);
    LogI& operator<<(const std::string &value);
};
class GLogWarning: public LogI{
public:
    static LogI& GetInstance() {
        static GLogWarning instance_;
        return instance_;
    };
    LogI& operator<<(int64_t value);
    LogI& operator<<(const std::string &value);
};
class GLogError: public LogI{
public:
    static LogI& GetInstance() {
        static GLogError instance_;
        return instance_;
    };
    LogI& operator<<(int64_t value);
    LogI& operator<<(const std::string &value);
};
class GLogFatal: public LogI{
public:
    static LogI& GetInstance() {
        static GLogFatal instance_;
        return instance_;
    };
    LogI& operator<<(int64_t value);
    LogI& operator<<(const std::string &value);
};
}
