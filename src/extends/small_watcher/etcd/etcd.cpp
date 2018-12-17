#include "etcd.h"
#include "small_watcher.h"

#include "connection_pool_manager.h"
#include "small_http_client.h"

#include "log.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "small_timer_factory.h"

namespace small_watcher{
Etcd::Etcd(const std::vector<std::string> &ips, uint32_t port):
    ips_(ips), port_(port) {
}
Etcd::~Etcd() {
}
void Etcd::Init(std::string &errMsg) {
    for (auto &ip: ips_) {
        small_http_client::ConnectionPoolManager::GetInstance().add(ip, std::to_string(port_), 100);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //todo errMsg test
    errMsg = "";
}
void Etcd::GetLocalIp(std::string &ip) {
    auto &connectionPoolManager = small_http_client::ConnectionPoolManager::GetInstance();
    auto connectionPool = connectionPoolManager.get(ips_[0], std::to_string(port_));
    connectionPool->GetLocalIp(ip);
}
/*createEphemeral
 * ok
 *  sleep long to refresh
 * failed
 *  sleep short to createEphemeral
 *refresh
 * ok
 *  sleep long to refresh
 * failed
 *  sleep short to createEphemeral
 */
void Etcd::CreateEphemeral(const std::string &path, const std::string &value){
    createEphemeral(path, value, "");
}
void Etcd::createEphemeral(const std::string &path, const std::string &value, const std::string &errMsg){
    if (errMsg != "") {
        LOG(ERROR) << errMsg;
        nextCreateEphemral(path, path);
        return;
    }
    //TODO rand ips
    auto c = std::make_shared<small_http_client::Async>("PUT", ips_[0], std::to_string(port_), 
        "/v2/keys" + path, "value=" + value + "&ttl=12");
    auto headers = std::shared_ptr<small_http_client::Headers>(new small_http_client::Headers({
                            {"Content-Type","application/x-www-form-urlencoded"}
                        }));
    auto onDone = [this, path, value](const std::string &respStr, 
        const std::shared_ptr<std::map<std::string, std::string>> respHeaders,
        const std::string &errMsg){
        if (errMsg != "") {
            LOG(ERROR) << errMsg;
            nextCreateEphemral(path, path);
            return;
        }
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            LOG(ERROR) << parseErrMsg;
            //ignore err to refresh
            nextRefresh(path, value);
            return;
        }
        if (jsonDoc.HasMember("errorCode")) {
            auto errorCode = jsonDoc["errorCode"].GetUint64();
            if (jsonDoc.HasMember("message")) {
                auto message = jsonDoc["message"].GetString();
                LOG(ERROR) << "errorCode: " << errorCode << " message: " << message;
                nextCreateEphemral(path, value);
                return;
            }
            LOG(ERROR) << "errorCode: " << errorCode;
            nextCreateEphemral(path, value);
            return;
        }
        nextRefresh(path, value);
    };
    c->setHeaders(headers);
    c->doReq(onDone);
}
void Etcd::refresh(const std::string &path, const std::string &value, 
        const std::string &errMsg) {
    if (errMsg != "") {
        LOG(ERROR) << errMsg;
        nextCreateEphemral(path, value);
        return;
    }
    //TODO rand ips
    auto c = std::make_shared<small_http_client::Async>("PUT", ips_[0], std::to_string(port_), 
        "/v2/keys" + path, "value=" + value + "&ttl=12&refresh=true");
    auto headers = std::shared_ptr<small_http_client::Headers>(new small_http_client::Headers({
                            {"Content-Type","application/x-www-form-urlencoded"}
                        }));
    auto onDone = [this, path, value](const std::string &respStr, 
        const std::shared_ptr<std::map<std::string, std::string>> respHeaders,
        const std::string &errMsg){
        if (errMsg != "") {
            LOG(ERROR) << errMsg;
            nextCreateEphemral(path, path);
            return;
        }
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            LOG(ERROR) << parseErrMsg;
            //ignore err to refresh
            nextRefresh(path, value);
            return;
        }
        if (jsonDoc.HasMember("errorCode")) {
            auto errorCode = jsonDoc["errorCode"].GetUint64();
            if (jsonDoc.HasMember("message")) {
                auto message = jsonDoc["message"].GetString();
                LOG(ERROR) << "errorCode: " << errorCode << " message: " << message;
                nextCreateEphemral(path, value);
                return;
            }
            LOG(ERROR) << "errorCode: " << errorCode;
            nextCreateEphemral(path, value);
            return;
        }
        nextRefresh(path, value);
        //auto node = jsonDoc["node"].GetObject();
        //auto index = node["modifiedIndex"].GetUint64();
    };
    c->setHeaders(headers);
    c->doReq(onDone);
}
void Etcd::nextCreateEphemral(const std::string &path, const std::string &value) {
    //sleep short to create
    //LOG(INFO) << "short to create";
    auto t = small_timer::MakeTimer();
    t->AsyncWait(1000, [this, t, path, value](const std::string &errMsg) {
            createEphemeral(path, value, errMsg);
        });
}
void Etcd::nextRefresh(const std::string &path, const std::string &value) {
    //sleep long to refresh
    //LOG(INFO) << "long to refresh";
    auto t = small_timer::MakeTimer();
    t->AsyncWait(5000, [this, t, path, value](const std::string &errMsg) {
            refresh(path, value, errMsg);
        });
}
void Etcd::List(const std::string &path, onListFunc handler) {
    //TODO rand ips
    auto c = std::make_shared<small_http_client::Async>("GET", ips_[0], std::to_string(port_), 
        "/v2/keys" + path + "/", "");
    auto onDone = [this, path, handler](const std::string &respStr, 
        const std::shared_ptr<std::map<std::string, std::string>> respHeaders,
        const std::string &argErrMsg){
        std::string errMsg;
        if (argErrMsg != "") {
            handler(argErrMsg, 0, nullptr);
            return;
        }
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            handler(parseErrMsg, 0, nullptr);
            return;
        }
        if (jsonDoc.HasMember("errorCode")) {
            auto errorCode = jsonDoc["errorCode"].GetUint64();
            if (jsonDoc.HasMember("message")) {
                auto message = jsonDoc["message"].GetString();
                errMsg = "errorCode: " + std::to_string(errorCode) + " message: " + message;
                handler(errMsg, 0, nullptr);
                return;
            }
            errMsg = "errorCode: " + std::to_string(errorCode);
            handler(errMsg, 0, nullptr);
            return;
        }
        auto etcdIndexStr = (*respHeaders)["X-Etcd-Index"];
        int64_t etcdIndex = -1;
        etcdIndex = std::stoll(etcdIndexStr);
        if (etcdIndex == -1) {
            errMsg = "invalid etcdIndex " + etcdIndexStr;
            handler(errMsg, 0, nullptr);
            return;
        }
        auto keys = std::make_shared<std::vector<std::string>>();
        if (jsonDoc.HasMember("node")) {
            auto node = jsonDoc["node"].GetObject();
            if (node.HasMember("nodes")) {
                auto nodes = node["nodes"].GetArray();
                for (auto &n : nodes) {
                    if (n.HasMember("key")) {
                        auto key = n["key"].GetString();
                        keys->push_back(key);
                    }
                }
            }
        }
        handler(errMsg, etcdIndex + 1, keys);
    };
    c->doReq(onDone);
}
void Etcd::WatchOnce(const uint64_t afterIndex,
        const std::string &path, onNotifyFunc handler) {
    auto c = std::make_shared<small_http_client::Async>("GET", ips_[0], std::to_string(port_), 
        "/v2/keys" + path + "?wait=true&recursive=true", "");
    auto onDone = [this, path, handler](const std::string &respStr, 
        const std::shared_ptr<std::map<std::string, std::string>> respHeaders,
        const std::string &argErrMsg){
        handler(argErrMsg);
    };
    c->doReq(onDone);
}
void Etcd::ListWatch(const std::string &path, 
            const onListWatchFunc &func) {
    watch(0, func, [this, func](){
        listWatch(func);
    });
}
void Etcd::watch(const uint64_t afterIndex, const onListWatchFunc &func,
        const std::function<void()> &optFunc) {
    WatchOnce(afterIndex, "/dsp_se", [this, afterIndex, func, optFunc](const std::string &argErrMsg){
        if (!argErrMsg.empty()) {
            LOG(WARNING) << argErrMsg;
            auto t = small_timer::MakeTimer();
            t->AsyncWait(5000, [this, t, afterIndex, func, optFunc](const std::string &errMsg) {
                watch(afterIndex, func, optFunc);
            });
            return;
        }
        listWatch(func);
    });
}
void Etcd::listWatch(const onListWatchFunc &func) {
    List("/dsp_se", [this, func](const std::string &argErrMsg, uint64_t afterIndex, 
    std::shared_ptr<std::vector<std::string>> keys){
        bool stop = false;
        if (!argErrMsg.empty()) {
            LOG(WARNING) << argErrMsg;
        }else {
            func(argErrMsg, stop, keys);
        }
        if (stop) {
            return;
        }
        watch(afterIndex, func, [this, func](){
            listWatch(func);
        });
    });
}
}//namespace small_watcher
