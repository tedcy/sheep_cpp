#pragma once
#include "glog/glog.h"
#include "log_interface.h"

namespace small_log{
class GLogManager : public LogManagerI{
public:
    ~GLogManager();
    static GLogManager& GetInstance();
    void Init(const std::string &path, const std::string &name) override;
    void EnableTrace() override;
private:
    GLogManager() = default;
    GLogManager(const GLogManager&) = default;
    GLogManager& operator=(const GLogManager&) = default;
    std::string gName_;
    bool inited_ = false;
};
}
