#pragma once
#include "small_config_interface.h"
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <memory>
#include <stack>
#include <iostream>

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
    void Encode(const char *name, T &element, typename std::enable_if<std::is_unsigned<T>::value>::type* = 0) {
        int64_t element1 = element;
        Encode(name, element1);
    }
    template <typename T>
    void Encode(const char *name, T &element, typename std::enable_if<std::is_signed<T>::value>::type* = 0) {
        auto nowObj = nowObjs_.top();
        nowObj->AddMember(rapidjson::StringRef(name), element, allocator_);
    }
    template <typename T>
    void Encode(const char *name, T &element, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) {
        auto nowObj = nowObjs_.top();
        nowObj->AddMember(rapidjson::StringRef(name), element, allocator_);
    }
    void Encode(const char *name, bool &element) {
        auto nowObj = nowObjs_.top();
        nowObj->AddMember(rapidjson::StringRef(name), element, allocator_);
    }
    void Encode(const char *name, std::string &element) {
        auto nowObj = nowObjs_.top();
        nowObj->AddMember(rapidjson::StringRef(name),
        rapidjson::StringRef(element.c_str()), allocator_);
    }
    template <typename T>
    void Encode(const char *name, std::vector<T> &elements) {
        auto nowObj = nowObjs_.top();
        rapidjson::Value t_obj(rapidjson::kArrayType);
        for (auto &element: elements) {
            t_obj.PushBack(createJsonValue(element), allocator_);
        }
        nowObj->AddMember(rapidjson::StringRef(name), t_obj, allocator_);
    }
    void Encode(const char *name, JsonEncodeElementI &element) {
        auto nowObj = nowObjs_.top();
        rapidjson::Value t_obj(rapidjson::kObjectType);
        nowObjs_.push(&t_obj);
        element.Accept(*this);
        nowObjs_.pop();
        nowObj->AddMember(rapidjson::StringRef(name), t_obj, allocator_);
    }
    void GetResult(std::string &out) {
        document_.Accept(*writer_);
        out = strbuf_.GetString();
    }
private:
    template <typename T>
    rapidjson::Value createJsonValue(T &element, typename std::enable_if<std::is_integral<T>::value>::type* = 0) {
        rapidjson::Value value;
        value.Set(element);
        return value;
    }
    template <typename T>
    rapidjson::Value createJsonValue(T &element, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) {
        rapidjson::Value value;
        value.Set(element);
        return value;
    }
    rapidjson::Value createJsonValue(bool &element) {
        rapidjson::Value value;
        value.Set(element);
        return value;
    }
    rapidjson::Value createJsonValue(std::string &element) {
        rapidjson::Value value;
        value.SetString(element.c_str(), element.size());
        return value;
    }
    rapidjson::Value createJsonValue(JsonEncodeElementI &element) {
        auto nowObj = nowObjs_.top();
        rapidjson::Value t_obj(rapidjson::kObjectType);
        nowObjs_.push(&t_obj);
        element.Accept(*this);
        nowObjs_.pop();
        return t_obj;
    }
    rapidjson::Document::AllocatorType      allocator_;
    std::stack<rapidjson::Value*>           nowObjs_;

    rapidjson::StringBuffer                 strbuf_;
    std::unique_ptr<rapidjson::Writer<rapidjson::StringBuffer>> writer_;
    rapidjson::Document                     document_;
};

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
   
    //FIXME: int64_t possibly to overflow
    template <typename T>
    void Decode(const char* name, T &element, typename std::enable_if<std::is_unsigned<T>::value>::type* = 0) {
        int64_t element1 = element;
        Decode(name, element1);
        element = element1;
    }
    template <typename T>
    void Decode(const char* name, T &element, typename std::enable_if<std::is_signed<T>::value>::type* = 0) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].Is<T>()) {
                element = (*nowObj)[name].Get<T>();
            }
            else
                throw std::string(name) + " is invalid";
        }
    }
    template <typename T>
    void Decode(const char* name, T &element, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].Is<T>()) {
                element = (*nowObj)[name].Get<T>();
            }
            else
                throw std::string(name) + " is invalid";
        }
    }
    void Decode(const char* name, bool &element) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].Is<bool>()) {
                element = (*nowObj)[name].Get<bool>();
            }
            else
                throw std::string(name) + " is invalid";
        }
    }
    void Decode(const char* name, std::string &element) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].IsString()) {
                element = (*nowObj)[name].GetString();
            }
            else
                throw std::string(name) + " is not a string";
        }
    }
    template <typename T>
    void Decode(const char *name, std::vector<T> &elements) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if ((*nowObj)[name].IsArray()) {
                for (rapidjson::SizeType i = 0; i < (*nowObj)[name].Size(); i++) {
                    T t;
                    setT((*nowObj)[name][i], t);
                    elements.push_back(t);
                }
            }
            else
                throw std::string(name) + " is not a array";
        }
    }
    void Decode(const char* name, JsonDecodeElementI &element) {
        auto nowObj = nowObjs_.top();
        if(name && nowObj->IsObject() && nowObj->HasMember(name)){
            if(!(*nowObj)[name].IsObject())
                throw std::string("json wrong fomat object: ") + name;
            nowObjs_.push(&((*nowObj)[name]));
            element.Accept(*this);
            nowObjs_.pop();
        }
    }
private:
    template <typename T>
    void setT(rapidjson::Value &v, T &element, typename std::enable_if<std::is_integral<T>::value>::type* = 0) {
        element = v.GetInt64();
    }
    template <typename T>
    void setT(rapidjson::Value &v, T &element, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) {
        element = v.GetDouble();
    }
    void setT(rapidjson::Value &v, bool &element) {
        element = v.GetBool();
    }
    void setT(rapidjson::Value &v, std::string &element) {
        element = std::string(v.GetString(), v.GetStringLength());
    }
    void setT(rapidjson::Value &v, JsonDecodeElementI &element) {
        auto nowObj = nowObjs_.top();
        if(nowObj->IsObject()){
            nowObjs_.push(&v);
            element.Accept(*this);
            nowObjs_.pop();
        }
    }
    std::string                             in_;
    rapidjson::Document::AllocatorType      allocator_;
    std::stack<rapidjson::Value*>           nowObjs_;
    rapidjson::Document                     document_;
};
}
}
