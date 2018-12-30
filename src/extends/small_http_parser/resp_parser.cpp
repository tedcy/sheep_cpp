#include "small_http_parser.h"
#include "http-parser/http_parser.h"
#include "log.h"
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
    parser_->http_errno = 0;
    inited_ = true;
    settings_->on_chunk_complete = nullptr;
    settings_->on_chunk_header = nullptr;
    settings_->on_headers_complete = nullptr;
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
    auto offset = buf_.size();
    buf_ += str;
    int nparsed = http_parser_execute(parser_.get(), settings_.get(), buf_.c_str() + offset, str.size());
    if (parser_->http_errno != 0) {
        errMsg = std::string("RespParser Feed failed: ") +
            ::http_errno_description(static_cast<enum ::http_errno>(parser_->http_errno));
        finished = false;
        return;
    }
    if (nparsed != str.size()) {
        errMsg = "RespParser Feed failed: unknown";
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
int RespParser::GetStatusCode() {
    return parser_->status_code;
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
    t->finished_ = true;
    return 0;
}
}
