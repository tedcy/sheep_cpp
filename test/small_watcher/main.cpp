#include "small_watcher.h"
#include "small_client.h"
#include "log.h"

#include <chrono>
#include <thread>
#include <string>

struct Test{
    Test() {
        //small_log::Init();
        small_client::Looper::GetInstance()->Init();
        std::vector<std::string> ips{"172.16.187.149"};
        uint32_t port = 2379;
        watcher_ = small_watcher::MakeWatcher(ips, port);
    }
    ~Test() {
        std::this_thread::sleep_for(std::chrono::seconds(100));
        small_client::Looper::GetInstance()->Shutdown();
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
        watcher_->ListWatch("/dsp_se", [](const std::string &argErrMsg, bool &stop,
            std::shared_ptr<std::vector<std::string>> keys){
            if (!argErrMsg.empty()) {
                LOG(WARNING) << argErrMsg;
                return;
            }
            LOG(INFO) << "new keys " << keys->size();
            for (auto &key: *keys) {
                LOG(INFO) << key;
            }
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
