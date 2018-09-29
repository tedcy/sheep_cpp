#pragma once

namespace small_log{
class LogI {
public:
    enum Level {
        DEBUG,
        WARNING,
        INFO,
        ERROR,
        FATAL,
    };
    virtual void Log(Level l) = 0;
};
}
