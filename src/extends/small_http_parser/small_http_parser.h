#pragma once
#include <map>
#include <string>
#include <memory>
#include "urlcode.h"
struct http_parser;
struct http_parser_settings;
namespace small_http_parser{
class Map{
public:
    Map() = default;
    explicit Map(const std::map<std::string, std::string> &kvs) :
        kvs_(kvs) {
    }
    void Set(const std::string &key, const std::string &value) {
        kvs_.insert({key, value});
    }
    void Get(const std::string &key, std::string &value) const {
        auto iter = kvs_.find(key);
        if (iter == kvs_.end()) {
            value = "";
        }
        value = iter->second;
    }
    std::map<std::string, std::string> kvs_;
};
//TODO RespFormater and ReqParser
class RespParser {
public:
    RespParser();
    void Feed(std::string &errMsg, bool &finished, const std::string &str);
    const Map& GetHeader();
    const std::string& GetBody();
    int GetStatusCode();
private:
    std::shared_ptr<http_parser_settings> settings_;
    std::shared_ptr<http_parser> parser_;
    Map headers_;
    std::string body_;
    std::string tmpField_;
    std::string buf_;

    void Init();
    static int onHeaderField(http_parser *, const char*, size_t);
    static int onHeaderValue(http_parser *, const char*, size_t);
    static int onBody(http_parser *, const char*, size_t);

    bool inited_ = false;
    bool finished_ = false;
};
class ReqFormater {
public:
    static const std::string MethodGET;
    static const std::string MethodPOST;
    static const std::string MethodPUT;
    ReqFormater(const std::string &method, const std::string &host,
        const std::string &target, const std::string &body);
    const std::string& Format();
    void SetHeader(const Map &map);
    void SetQueryString(const Map &map);
private:
    static const std::string LR;
    static const std::string Host;
    static const std::string ContentLength;
    std::string method_;
    std::string host_;
    std::string target_;
    std::string body_;
    std::string result_;
    Map headers_;
    Map queryStrings_;
};
}
