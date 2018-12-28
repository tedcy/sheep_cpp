#include "small_http_parser.h"
#include "http-parser/http_parser.h"
#include <iostream>
namespace small_http_parser{
RespParser::RespParser() :
    settings_(std::make_shared<http_parser_settings>()),
    parser_(std::make_shared<http_parser>()){
    http_parser_init(parser_.get(), HTTP_RESPONSE);
}
void RespParser::Init() {
    if (inited_) {
        return;
    }
    parser_->data = this;
    inited_ = true;
    settings_->on_chunk_complete = nullptr;
    settings_->on_chunk_header = nullptr;
    settings_->on_headers_complete = RespParser::onFeedFinished;
    settings_->on_message_begin = nullptr;
    settings_->on_message_complete = nullptr;
    settings_->on_url = nullptr;
    settings_->on_status = nullptr;
    settings_->on_header_field = RespParser::onHeaderField;
    settings_->on_header_value = RespParser::onHeaderValue;
    settings_->on_body = RespParser::onBody;
}
void RespParser::Feed(std::string &errMsg, bool &finished, const std::string &str) {
    Init();
    int nparsed = http_parser_execute(parser_.get(), settings_.get(), str.c_str(), str.size());
    if (nparsed != str.size()) {
        errMsg = "RespParser Feed failed";
        finished = false;
        return;
    }
    finished = finished_;
}
const Map& RespParser::GetHeader() {
    return headers_;
}
const std::string& RespParser::GetBody() {
    return body_;
}
const int RespParser::GetStatusCode() {
    return parser_->status_code;
}
int RespParser::onFeedFinished(http_parser *parser) {
    std::cout << "onFeedFinished" << std::endl;
    auto t = static_cast<RespParser*>(parser->data);
    t->finished_ = true;
    return 0;
}
int RespParser::onHeaderField(http_parser *parser, const char *c, size_t len) {
    auto t = static_cast<RespParser*>(parser->data);
    t->tmpField_ = std::string(c, len);
    return 0;
}
int RespParser::onHeaderValue(http_parser *parser, const char *c, size_t len) {
    auto t = static_cast<RespParser*>(parser->data);
    t->headers_.Set(t->tmpField_, std::string(c, len));
    return 0;
}
int RespParser::onBody(http_parser *parser, const char *c, size_t len) {
    auto t = static_cast<RespParser*>(parser->data);
    t->body_ = std::string(c, len);
    return 0;
}
}
