#pragma once
#include "glog.h"

namespace small_log{
class GLogManager {
public:
    ~GLogManager() = default;
    static GLogManager& GetInstance();
    LogI& Log(Level l);
    void Init(const std::string &path, const std::string &name);
    void EnableTrace();
private:
    GLogManager() = default;
    GLogManager(const GLogManager&) = default;
    GLogManager& operator=(const GLogManager&) = default;
    std::string gName_;
};
}
