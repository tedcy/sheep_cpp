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
        small_http_client::ConnectionPoolManager::getInstance()->add(ip, std::to_string(port_), 1);
    }
	small_http_client::ConnectionPoolManager::getInstance()->work();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //todo errMsg test
    errMsg = "";
}
void Etcd::GetLocalIp(std::string &ip) {
    auto connectionPoolManager = small_http_client::ConnectionPoolManager::getInstance();
    auto connectionPool = connectionPoolManager->get(ips_[0], std::to_string(port_));
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
    t->AsyncWait(1000, [this, path, value](const std::string &errMsg) {
            createEphemeral(path, value, errMsg);
        });
}
void Etcd::nextRefresh(const std::string &path, const std::string &value) {
    //sleep long to refresh
    //LOG(INFO) << "long to refresh";
    auto t = small_timer::MakeTimer();
    t->AsyncWait(5000, [this, path, value](const std::string &errMsg) {
            refresh(path, value, errMsg);
        });
}
}//namespace small_watcher
