#include "small_http_parser.h"
#include "log.h"
namespace small_http_parser{
const std::string ReqFormater::MethodGET = "GET";
const std::string ReqFormater::MethodPOST = "POST";
const std::string ReqFormater::MethodPUT = "PUT";
const std::string ReqFormater::LR = "\r\n";
const std::string ReqFormater::Host = "Host";
const std::string ReqFormater::ContentLength = "Content-Length";
ReqFormater::ReqFormater(const std::string &method,
        const std::string &target,
        const std::string &body) :
    method_(method),target_(target),
    body_(body) {
}
const std::string& ReqFormater::Format() {
    std::string flag;

    //query string
    bool first = true;
    for (auto &queryString : queryStrings_.kvs_) {
        if (first) {
            flag = "?";
            first = false;
        }else {
            flag = "&";
        }
        target_ += (flag + UrlEncode(queryString.first) + "=" + UrlEncode(queryString.second));
    }
    result_ = method_ + " " + target_ + " HTTP/1.1" + LR;
    
    //header
    if (!host_.empty()) {
        headers_.Set(Host, host_);
    }
    if (method_ == ReqFormater::MethodPUT || method_ == ReqFormater::MethodPOST) {
        headers_.Set(ContentLength, std::to_string(body_.size()));
    }
    for (auto &header: headers_.kvs_) {
        result_ += (header.first + ": " + header.second + LR);
    }
    result_ += LR;

    //body
    if (method_ == ReqFormater::MethodPUT || method_ == ReqFormater::MethodPOST) {
        result_ += body_;
    }
    return result_;
}
void ReqFormater::SetHost(const std::string &host) {
    host_ = host;
}
void ReqFormater::SetHeader(const Map &map) {
    headers_ = map;
}
void ReqFormater::SetQueryString(const Map &map) {
    queryStrings_ = map;
}
}
