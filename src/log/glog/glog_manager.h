#pragma once
#include "glog/glog.h"
#include "log_interface.h"

namespace small_log{
class GLogFactory : public LogFactoryI{
public:
    ~GLogFactory();
    static GLogFactory& GetInstance();
    void Init(const std::string &path, const std::string &name) override;
    void EnableTrace() override;
private:
    GLogFactory() = default;
    GLogFactory(const GLogFactory&) = default;
    GLogFactory& operator=(const GLogFactory&) = default;
    std::string gName_;
};
}
