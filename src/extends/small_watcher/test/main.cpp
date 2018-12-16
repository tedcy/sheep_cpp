#include "small_watcher_factory.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>

#include "small_net.h"

struct Test{
    Test() {
        //small_log::Init();
        small_net::AsioNet::GetInstance().Init();
        std::vector<std::string> ips{"172.16.187.149"};
        uint32_t port = 2379;
        watcher_ = small_watcher::MakeWatcher(ips, port);
    }
    ~Test() {
        std::this_thread::sleep_for(std::chrono::seconds(100));
        small_net::AsioNet::GetInstance().Shutdown();
    }
    void Init(std::string &errMsg) {
        watcher_->Init(errMsg);
        if (errMsg != "") {
            LOG(FATAL) << errMsg;
        }
    }
    void TestCreateEphemeral() {
        watcher_->CreateEphemeral("/test_watcher", "");
    }
    void TestList() {
        watcher_->List("/dsp_se", [](const std::string &argErrMsg, uint64_t afterIndex, 
            std::shared_ptr<std::vector<std::string>> keys){
            if (!argErrMsg.empty()) {
                LOG(FATAL) << argErrMsg;
            }
            LOG(INFO) << afterIndex;
            for (auto &key: *keys) {
                LOG(INFO) << key;
            }
        });
    }
    void TestListWatcher() {
        testListWatcher1(0);
    }
    void testListWatcher1(uint64_t afterIndex) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        watcher_->Watch(afterIndex, "/dsp_se", [this, afterIndex](const std::string &argErrMsg){
            if (!argErrMsg.empty()) {
                LOG(WARNING) << argErrMsg;
                testListWatcher1(afterIndex);
                return;
            }
            testListWatcher2();
        });
    }
    void testListWatcher2() {
        watcher_->List("/dsp_se", [this](const std::string &argErrMsg, uint64_t afterIndex, 
            std::shared_ptr<std::vector<std::string>> keys){
            if (!argErrMsg.empty()) {
                LOG(WARNING) << argErrMsg;
            }else {
                LOG(INFO) << afterIndex;
                for (auto &key: *keys) {
                    LOG(INFO) << key;
                }
            }
            testListWatcher1(afterIndex);
        });
    }
    std::shared_ptr<small_watcher::WatcherI> watcher_;
};

int main() {
    std::string errMsg;
    Test t;
    t.Init(errMsg);
    //t.TestCreateEphemeral();
    //t.TestList();
    t.TestListWatcher();
}
