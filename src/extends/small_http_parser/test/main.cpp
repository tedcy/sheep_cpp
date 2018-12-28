#include "small_http_parser.h"
#include "log.h"

void TestRespParser() {
    small_http_parser::RespParser parser;
    const std::string buf =
	"HTTP/1.1 200 OK\r\n"
         "Date: Tue, 04 Aug 2009 07:59:32 GMT\r\n"
         "Server: Apache\r\n"
         "X-Powered-By: Servlet/2.5 JSP/2.1\r\n"
         "Content-Type: text/xml; charset=utf-8\r\n"
         "Connection: close\r\n"
         "\r\n"
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
         "  <SOAP-ENV:Body>\n"
         "    <SOAP-ENV:Fault>\n"
         "       <faultcode>SOAP-ENV:Client</faultcode>\n"
         "       <faultstring>Client Error</faultstring>\n"
         "    </SOAP-ENV:Fault>\n"
         "  </SOAP-ENV:Body>\n"
         "</SOAP-ENV:Envelope>";
    std::string errMsg;
    bool finished;
	parser.Feed(errMsg, finished, buf);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    if (!finished) {
        LOG(FATAL) << "not finished";
    }
    auto statusCode = parser.GetStatusCode();
    LOG(INFO) << statusCode;
    auto headers = parser.GetHeader();
    for (auto &header : headers.kvs_) {
        LOG(INFO) << header.first << ":" << header.second;
    }
    auto resp = parser.GetBody();
    LOG(INFO) << resp;
}

void TestReqFormater() {
    small_http_parser::ReqFormater formater(small_http_parser::ReqFormater::MethodPOST
    , "www.baidu.com", "/", "wtf");
    small_http_parser::Map headers;
    headers.Set("Accept", "*/*");
    headers.Set("Connection", "keep-alive");
    formater.SetHeader(headers);

    small_http_parser::Map queryStrings;
    queryStrings.Set("a ","a");
    queryStrings.Set("b","b ");
    formater.SetQueryString(queryStrings);

    LOG(INFO) << formater.Format();
}

int main() {
    //TestRespParser();
    TestReqFormater();
}
