#include "etcd.h"
#include "small_watcher.h"

#include "log.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace small_watcher{
Etcd::Etcd(const std::vector<std::string> &ips, uint32_t port):
    ips_(ips), port_(port) {
}
Etcd::~Etcd() {
}
void Etcd::Init(std::string &errMsg) {
    core_ = std::make_shared<small_client::SheepNetClientCore>(
            small_client::SheepNetCore::GetInstance()->GetLoop());
    core_->SetResolverType("string");
    core_->SetMaxSize(10);
    core_->Init(errMsg, ips_, port_, "");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
void Etcd::GetLocalIp(std::string &ip) {
    bool ok;
    auto clientPool = core_->GetClientPool(ok);
    if (!ok) {
        LOG(FATAL) << "Etcd can't get LocalIp";
    }
    auto client = clientPool->Get();
    if (client == nullptr) {
        LOG(FATAL) << "Etcd can't get LocalIp";
    }
    auto &connection = client->GetTcpConnection();
    std::string errMsg;
    connection.GetLocalIp(errMsg, ip);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    clientPool->Insert(client);
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
    auto httpClient = std::make_shared<small_client::HttpClient>(*core_.get(), "PUT", ips_[0],
        "/v2/keys" + path, "value=" + value + "&ttl=12");
    httpClients_.insert(httpClient);
    auto weakPtr = std::weak_ptr<small_client::HttpClient>(httpClient);
    small_http_parser::Map headers;
    headers.Set("Content-Type","application/x-www-form-urlencoded");
    auto onDone = [this, weakPtr, path, value](small_client::HttpClient &client,
        const std::string &errMsg) {
        if (errMsg != "") {
            LOG(ERROR) << errMsg;
            nextCreateEphemral(path, path);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        auto &respStr = client.GetRespStr();
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            LOG(ERROR) << parseErrMsg;
            //ignore err to refresh
            nextRefresh(path, value);
            httpClients_.erase(weakPtr.lock());
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
            httpClients_.erase(weakPtr.lock());
            return;
        }
        nextRefresh(path, value);
    };
    httpClient->SetHeaders(headers);
    httpClient->DoReq(onDone);
}
void Etcd::refresh(const std::string &path, const std::string &value, 
        const std::string &errMsg) {
    if (errMsg != "") {
        LOG(ERROR) << errMsg;
        nextCreateEphemral(path, value);
        return;
    }
    auto httpClient = std::make_shared<small_client::HttpClient>(*core_.get(),
            "PUT", ips_[0], "/v2/keys" + path, "value=" + value + "&ttl=12&refresh=true");
    httpClients_.insert(httpClient);
    auto weakPtr = std::weak_ptr<small_client::HttpClient>(httpClient);
    small_http_parser::Map headers;
    headers.Set("Content-Type","application/x-www-form-urlencoded");
    auto onDone = [this, weakPtr, path, value](small_client::HttpClient &httpClient, 
        const std::string &errMsg){
        if (errMsg != "") {
            LOG(ERROR) << errMsg;
            nextCreateEphemral(path, path);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        auto &respStr = httpClient.GetRespStr();
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            LOG(ERROR) << parseErrMsg;
            //ignore err to refresh
            nextRefresh(path, value);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        if (jsonDoc.HasMember("errorCode")) {
            auto errorCode = jsonDoc["errorCode"].GetUint64();
            if (jsonDoc.HasMember("message")) {
                auto message = jsonDoc["message"].GetString();
                LOG(ERROR) << "errorCode: " << errorCode << " message: " << message;
                nextCreateEphemral(path, value);
                httpClients_.erase(weakPtr.lock());
                return;
            }
            LOG(ERROR) << "errorCode: " << errorCode;
            nextCreateEphemral(path, value);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        nextRefresh(path, value);
        httpClients_.erase(weakPtr.lock());
        //auto node = jsonDoc["node"].GetObject();
        //auto index = node["modifiedIndex"].GetUint64();
    };
    httpClient->SetHeaders(headers);
    httpClient->DoReq(onDone);
}
void Etcd::nextCreateEphemral(const std::string &path, const std::string &value) {
    //sleep short to create
    //LOG(INFO) << "short to create";
    auto t = std::make_shared<sheep::net::Timer>(small_client::SheepNetCore::GetInstance()->GetLoop());
    timers_.insert(t);
    auto weakPtr = std::weak_ptr<sheep::net::Timer>(t);
    t->AsyncWait(1000, [this, weakPtr, path, value](const std::string &errMsg) {
        createEphemeral(path, value, errMsg);
        timers_.erase(weakPtr.lock());
    });
}
void Etcd::nextRefresh(const std::string &path, const std::string &value) {
    //sleep long to refresh
    //LOG(INFO) << "long to refresh";
    auto t = std::make_shared<sheep::net::Timer>(small_client::SheepNetCore::GetInstance()->GetLoop());
    timers_.insert(t);
    auto weakPtr = std::weak_ptr<sheep::net::Timer>(t);
    t->AsyncWait(5000, [this, weakPtr, path, value](const std::string &errMsg) {
        refresh(path, value, errMsg);
        timers_.erase(weakPtr.lock());
    });
}
void Etcd::List(const std::string &path, onListFunc handler) {
    auto httpClient = std::make_shared<small_client::HttpClient>(*core_.get(),
        "GET", ips_[0],  "/v2/keys" + path + "/", "");
    httpClients_.insert(httpClient);
    auto weakPtr = std::weak_ptr<small_client::HttpClient>(httpClient);
    auto onDone = [this, weakPtr, path, handler](small_client::HttpClient &client, 
        const std::string &argErrMsg){
        std::string errMsg;
        if (argErrMsg != "") {
            handler(argErrMsg, 0, nullptr);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        auto &respStr = client.GetRespStr();
        auto &respHeaders = client.GetRespHeader();
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(respStr.c_str());
        if (jsonDoc.HasParseError()) {
            auto parseErrMsg = ("parse " + respStr +
                " Failed: code " + std::to_string(jsonDoc.GetParseError()) +
                " offset " + std::to_string(jsonDoc.GetErrorOffset()));
            handler(parseErrMsg, 0, nullptr);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        if (jsonDoc.HasMember("errorCode")) {
            auto errorCode = jsonDoc["errorCode"].GetUint64();
            if (jsonDoc.HasMember("message")) {
                auto message = jsonDoc["message"].GetString();
                errMsg = "errorCode: " + std::to_string(errorCode) + " message: " + message;
                handler(errMsg, 0, nullptr);
                httpClients_.erase(weakPtr.lock());
                return;
            }
            errMsg = "errorCode: " + std::to_string(errorCode);
            handler(errMsg, 0, nullptr);
            httpClients_.erase(weakPtr.lock());
            return;
        }
        std::string etcdIndexStr;
        respHeaders.Get("X-Etcd-Index", etcdIndexStr);
        int64_t etcdIndex = -1;
        etcdIndex = std::stoll(etcdIndexStr);
        if (etcdIndex == -1) {
            errMsg = "invalid etcdIndex " + etcdIndexStr;
            handler(errMsg, 0, nullptr);
            httpClients_.erase(weakPtr.lock());
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
        httpClients_.erase(weakPtr.lock());
    };
    httpClient->DoReq(onDone);
}
void Etcd::WatchOnce(const uint64_t afterIndex,
        const std::string &path, onNotifyFunc handler) {
    auto httpClient = std::make_shared<small_client::HttpClient>(*core_.get(),
            "GET", ips_[0], "/v2/keys" + path + "?wait=true&recursive=true", "");
    httpClients_.insert(httpClient);
    auto weakPtr = std::weak_ptr<small_client::HttpClient>(httpClient);
    auto onDone = [this, weakPtr, path, handler](small_client::HttpClient &client, 
        const std::string &argErrMsg){
        handler(argErrMsg);
        httpClients_.erase(weakPtr.lock());
    };
    httpClient->DoReq(onDone);
}
/*ListWatch
 *  listWatch
 *    List
 *    ok
 *      cb
 *      watch
 *    failed
 *      watch
 *
 *  watch
 *     WatchOnce
 *     ok
 *       listWatch
 *     failed
 *       sleep to watch
 */
void Etcd::ListWatch(const std::string &path, 
            const onListWatchFunc &func) {
    listWatch(path, func);
}
void Etcd::watch(const uint64_t afterIndex, 
        const std::string &path,
        const onListWatchFunc &func,
        const std::function<void()> &optFunc) {
    WatchOnce(afterIndex, path, [this, afterIndex, path, func, optFunc](const std::string &argErrMsg){
        if (!argErrMsg.empty()) {
            LOG(WARNING) << argErrMsg;
            auto t = std::make_shared<sheep::net::Timer>(small_client::SheepNetCore::GetInstance()->GetLoop());
            timers_.insert(t);
            auto weakPtr = std::weak_ptr<sheep::net::Timer>(t);
            t->AsyncWait(5000, [this, weakPtr, path, afterIndex, func, optFunc](const std::string &errMsg) {
                watch(afterIndex, path, func, optFunc);
                timers_.erase(weakPtr.lock());
            });
            return;
        }
        listWatch(path, func);
    });
}
void Etcd::listWatch(const std::string &path, const onListWatchFunc &func) {
    List(path, [this, path, func](const std::string &argErrMsg, uint64_t afterIndex, 
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
        watch(afterIndex, path, func, [this, path, func](){
            listWatch(path, func);
        });
    });
}
}//namespace small_watcher
