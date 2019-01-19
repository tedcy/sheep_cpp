#pragma once
#include "small_config_interface.h"
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <memory>
#include <stack>

namespace small_config{

namespace internal{
class JsonEncodeVisitor;
using JsonEncodeElementI = EncodeElementI<JsonEncodeVisitor>;
class JsonEncodeVisitor {
public:
    JsonEncodeVisitor(){
        document_.SetObject();
        writer_ = std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>>(new rapidjson::Writer<rapidjson::StringBuffer>(strbuf_));
        nowObjs_.push(&document_);
    }
    template <typename T>
    void Encode(const char* name, T &element) {
        EncodeImp(name, element, std::is_base_of<JsonEncodeElementI, T>());
    }
    void GetResult(std::string &out) {
        document_.Accept(*writer_);
        out = strbuf_.GetString();
    }
private:
    template <typename T> 
    inline void EncodeImp(const char* name, T& element, std::true_type) {
        auto nowObj = nowObjs_.top();
        rapidjson::Value t_obj(rapidjson::kObjectType);
        nowObjs_.push(&t_obj);
        element.Accept(*this);
        nowObjs_.pop();
        nowObj->AddMember(rapidjson::StringRef(name), t_obj, allocator_);
    }
    template <typename T> 
    inline void EncodeImp(const char* name, T& element, std::false_type) {
        auto nowObj = nowObjs_.top();
        nowObj->AddMember(rapidjson::StringRef(name), element, allocator_);
    }
    rapidjson::Document::AllocatorType      allocator_;
    std::stack<rapidjson::Value*>           nowObjs_;

    rapidjson::StringBuffer                 strbuf_;
    std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer_;
    rapidjson::Document                     document_;
};
template void JsonEncodeVisitor::Encode(const char* name, int32_t &element);
template void JsonEncodeVisitor::Encode(const char* name, int64_t &element);
template void JsonEncodeVisitor::Encode(const char* name, bool &element);
template void JsonEncodeVisitor::Encode(const char* name, double &element);
template <> 
inline void JsonEncodeVisitor::Encode(const char* name, uint32_t &element) {
    int64_t element1 = element;
    Encode(name, element1);
}
//FIXME: possibly to overflow
template <> 
inline void JsonEncodeVisitor::Encode(const char* name, uint64_t &element) {
    int64_t element1 = element;
    Encode(name, element1);
}
template <> 
inline void JsonEncodeVisitor::Encode(const char* name, std::string& element) {
    auto nowObj = nowObjs_.top();
    nowObj->AddMember(rapidjson::StringRef(name),
        rapidjson::StringRef(element.c_str()), allocator_);
}

class JsonDecodeVisitor;
using JsonDecodeElementI = DecodeElementI<JsonDecodeVisitor>;
class JsonDecodeVisitor {
public:
    JsonDecodeVisitor(const std::string &in):
        in_(in) {
    }
    void Parse(std::string &errMsg) {
        if (document_.Parse<0>(in_.c_str()).HasParseError()) {
            errMsg = "json obj has parse error";
        }
        nowObjs_.push(&document_);
    }
    
    template <typename T>
    void Decode(const char* name, T& element) {
        DecodeImp(name, element, std::is_base_of<JsonDecodeElementI, T>());
    }
private:
    template <typename T> 
    inline void DecodeImp(const char* name, T& element, std::true_type) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if(!(*nowObj)[name].IsObject())
                throw std::string("json wrong fomat object: ") + name;
            nowObjs_.push(&((*nowObj)[name]));
            element.Accept(*this);
            nowObjs_.pop();
        }else{
            throw std::string("json has not a member of ") + name;
        }
    }
    template <typename T> 
    inline void DecodeImp(const char* name, T& element, std::false_type) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].Is<T>()) {
                element = (*nowObj)[name].Get<T>();
            }
            else
                throw std::string(name) + " is invalid";
        }else
            throw std::string("json has not a member of ") + name;
    }
    std::string                             in_;
    rapidjson::Document::AllocatorType      allocator_;
    std::stack<rapidjson::Value*>           nowObjs_;
    rapidjson::Document                     document_;
};

template void JsonDecodeVisitor::Decode(const char* name, int32_t& element);
template void JsonDecodeVisitor::Decode(const char* name, int64_t& element);
template void JsonDecodeVisitor::Decode(const char* name, bool& element);
template void JsonDecodeVisitor::Decode(const char* name, double& element);
template <> 
inline void JsonDecodeVisitor::Decode(const char* name, uint32_t& element) {
    int64_t element1 = element;
    Decode(name, element1);
    element = element1;
}
//FIXME: possibly to overflow
template <> 
inline void JsonDecodeVisitor::Decode(const char* name, uint64_t& element) {
    int64_t element1 = element;
    Decode(name, element1);
    element = element1;
}
template <> 
inline void JsonDecodeVisitor::Decode(const char* name, std::string& element) {
    auto nowObj = nowObjs_.top();
    if(name && nowObj->IsObject() && nowObj->HasMember(name)){
        if ((*nowObj)[name].IsString()) {
            element = (*nowObj)[name].GetString();
        }
        else
            throw std::string(name) + " is not a string";
    }else
        throw std::string("json has not a member of ") + name;
}
}
}
