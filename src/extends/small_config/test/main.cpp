#include "small_config.h"
#include "log.h"


struct Test : public small_config::JsonClient,
    public small_config::TomlClient {
    struct NestTest: public small_config::JsonClient,
        public small_config::TomlClient {
        std::string NestAddr;
        void Accept(small_config::JsonEncodeVisitor &v) override{
            v.Encode("nest_addr", NestAddr);
        }
        void Accept(small_config::JsonDecodeVisitor &v) override{
            v.Decode("nest_addr", NestAddr);
        }
        void Accept(small_config::TomlEncodeVisitor &v) override{
            v.Encode("nest_addr", NestAddr);
        }
        void Accept(small_config::TomlDecodeVisitor &v) override{
            v.Decode("nest_addr", NestAddr);
        }
    };
    std::string Addr;
    uint64_t    Port;
    NestTest    nest;
    void Accept(small_config::JsonEncodeVisitor &v) override{
        v.Encode("addr", Addr);
        v.Encode("port", Port);
        v.Encode("nest", nest);
    }
    void Accept(small_config::JsonDecodeVisitor &v) override{
        v.Decode("addr", Addr);
        v.Decode("port", Port);
        v.Decode("nest", nest);
    }
    void Accept(small_config::TomlEncodeVisitor &v) override{
        v.Encode("addr", Addr);
        v.Encode("port", Port);
        v.Encode("nest", nest);
    }
    void Accept(small_config::TomlDecodeVisitor &v) override{
        v.Decode("addr", Addr);
        v.Decode("port", Port);
        v.Decode("nest", nest);
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
    t.nest.NestAddr = "nestAddr_";
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
    LOG(INFO) << t1.Addr;
    LOG(INFO) << t1.Port;
    LOG(INFO) << t1.nest.NestAddr;
}

int main() {
    test1();
    test2();
}
