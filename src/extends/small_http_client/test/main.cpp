#include "small_http_client.h"
#include "connection_pool_manager.h"
#include "log.h"

#include <json/json.h>

#include <memory>
#include <thread>
#include <iostream>

//#include <boost/asio/connect.hpp>
//#include <boost/asio/ip/tcp.hpp>
#include "small_net.h"

int main() {
    //TODO
    //当不进行最后的sleep时，会存在core
    //估计是部分内容析构时继续运行了被析构的对象内容导致的
    //small_log::Init();
    small_log::EnableTrace();
    small_net::AsioNet::GetInstance().Init();
    //small_http_client::ConnectionPoolManager::getInstance()->add("api.orion.meizu.com", "80", 1);
    small_http_client::ConnectionPoolManager::GetInstance().add("127.0.0.1", "8081", 1);
    std::this_thread::sleep_for(std::chrono::seconds(1));

	Json::Value dmpReq;
    dmpReq["outTags"].append("sex");
    dmpReq["outTags"].append("user_age");
    dmpReq["outTags"].append("user_job");
    dmpReq["uid"] = "123";
    auto reqStr = dmpReq.toStyledString();

    auto queryStrings = std::shared_ptr<small_http_client::QueryStrings>(new small_http_client::QueryStrings({
            {"reqid","860755011210976"},
            {"openid","ad"},
            {"token","124154"},
            {"ts","1527842426"},
            {"uid","dsp"}
        }));
    auto headers = std::shared_ptr<small_http_client::Headers>(new small_http_client::Headers({
            {"Content-Type","application/json"}
        }));

    
    
    for (int i = 0;i < 3;i++) {
        std::shared_ptr<small_http_client::Async> c = std::make_shared<small_http_client::Async>("POST",
            //"api.orion.meizu.com", 
            //"80", 
            "127.0.0.1", 
            "8081", 
            "/dmp/api/tag/getTagsByUserId",
            reqStr);
        auto onDone = [](const std::string &respStr, 
                const std::shared_ptr<std::map<std::string, std::string>>,
                const std::string &errMsg) {
            LOG(INFO) << respStr;
            if (errMsg != "") {
                LOG(ERROR) << errMsg;
                return;
            }
            return;
            Json::Value resp;
		    Json::CharReaderBuilder builder;
            builder["collectComments"] = false;
            auto reader = builder.newCharReader();
            std::string errMsg1;
            auto b = (char*)respStr.c_str();
            if (!reader->parse(b, b + respStr.size(), &resp, &errMsg1)) {
                LOG(ERROR) << errMsg1;
                delete(reader);
                return;
            }
            LOG(INFO) << resp["code"].asString();
		    auto userProfiles = resp["tags"];
    	    auto mem = userProfiles.getMemberNames();
    	    for (auto iter = mem.begin(); iter != mem.end(); iter++) {
       		    std::string key = *iter;
       		    std::string value = userProfiles[key].asString();
                LOG(INFO) << key << "\t" << value;
		    }
            delete(reader);
            return;
        };
        c->setQueryStrings(queryStrings);
        c->SetReadTimeout(200);
        c->SetWriteTimeout(200);
        c->setHeaders(headers);
        c->doReq(onDone);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(20));
    small_net::AsioNet::GetInstance().Shutdown();
}
