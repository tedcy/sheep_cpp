#include "small_config.h"
#include "log.h"

struct Test : public small_config::JsonClient,
    public small_config::TomlClient {
    std::string Addr;
    uint64_t     Port;
    void Accept(small_config::JsonEncodeVisitor &v) override{
        v.Encode("addr", Addr);
        v.Encode("port", Port);
    }
    void Accept(small_config::JsonDecodeVisitor &v) override{
        v.Decode("addr", Addr);
        v.Decode("port", Port);
    }
    void Accept(small_config::TomlEncodeVisitor &v) override{
        v.Encode("addr", Addr);
        v.Encode("port", Port);
    }
    void Accept(small_config::TomlDecodeVisitor &v) override{
        v.Decode("addr", Addr);
        v.Decode("port", Port);
    }
};

void test1() {
    Test t;
    std::string errMsg;
    t.UnSerializeTomlFromFile(errMsg, "test.toml");
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << "UnSerialize file";
    LOG(INFO) << t.Addr;
    LOG(INFO) << t.Port;
}
void test2() {
    Test t;
    t.Addr = "addr_";
    t.Port = 1;
    std::string errMsg;
    std::string out;
    t.SerializeToml(errMsg, out); 
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << "Serialize";
    LOG(INFO) << out;
    Test t1;
    t1.UnSerializeToml(errMsg, out);
    if (!errMsg.empty()) {
        LOG(FATAL) << errMsg;
    }
    LOG(INFO) << "UnSerialize";
    LOG(INFO) << t1.Addr << "\t" << t1.Port;
}

int main() {
    test1();
    test2();
}
