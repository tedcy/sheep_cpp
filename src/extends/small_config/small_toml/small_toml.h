#pragma once

#include "small_config_interface.h"
#include <map>
#include <stack>
#include <iostream>

#include "toml/toml.h"

namespace small_config{

namespace internal{
class TomlEncodeVisitor;
using TomlEncodeElementI = EncodeElementI<TomlEncodeVisitor>;
class TomlEncodeVisitor {
public:
    TomlEncodeVisitor() {
        nowObjs_.push(&rootObj_);
    }
    template <typename T>
    void Encode(const char* name, T &element) {
        auto nowObj = nowObjs_.top();
        nowObj->setChild(name, element);
    }
    void GetResult(std::string &out) {
        std::stringstream ss;
        ss << rootObj_;
        out = ss.str();
    }
private:
    toml::Value rootObj_;
    std::stack<toml::Value*> nowObjs_;
};
template void TomlEncodeVisitor::Encode(const char* name, int32_t &element);
template void TomlEncodeVisitor::Encode(const char* name, int64_t &element);
template void TomlEncodeVisitor::Encode(const char* name, bool &element);
template void TomlEncodeVisitor::Encode(const char* name, double &element);
template void TomlEncodeVisitor::Encode(const char* name, std::string& element);
template <> 
inline void TomlEncodeVisitor::Encode(const char* name, uint32_t &element) {
    int64_t element1 = element;
    Encode(name, element1);
}
//FIXME: possibly to overflow
template <> 
inline void TomlEncodeVisitor::Encode(const char* name, uint64_t &element) {
    int64_t element1 = element;
    Encode(name, element1);
}
template <>
inline void TomlEncodeVisitor::Encode(const char* name, TomlEncodeElementI& element) {
    auto nowObj = nowObjs_.top();
    toml::Value obj;
    nowObjs_.push(&obj);
    element.Accept(*this);
    nowObjs_.pop();
    nowObj->setChild(name, obj);
}

class TomlDecodeVisitor;
using TomlDecodeElementI = DecodeElementI<TomlDecodeVisitor>;
class TomlDecodeVisitor {
public:
    explicit TomlDecodeVisitor(const std::string &in) :
        ss_(in){
    }
    void Parse(std::string &errMsg) {
        result_ = std::unique_ptr<toml::ParseResult>(
                new toml::ParseResult(toml::parse(ss_)));
        if (!result_->valid()) {
            errMsg = result_->errorReason;
            return;
        }
        rootObj_ = &result_->value;
        nowObjs_.push(rootObj_);
    }
    template <typename T>
    void Decode(const char* name, T &element) {
        auto nowObj = nowObjs_.top();
        auto v = nowObj->find(name);
        if (v) {
            if (v->is<T>()) {
                element = v->as<T>();
            }
        }
    }
private:
    std::stringstream ss_;
    std::unique_ptr<toml::ParseResult> result_;
    const toml::Value *rootObj_;
    std::stack<const toml::Value*> nowObjs_;
};
template void TomlDecodeVisitor::Decode(const char* name, int32_t &element);
template void TomlDecodeVisitor::Decode(const char* name, int64_t &element);
template void TomlDecodeVisitor::Decode(const char* name, bool &element);
template void TomlDecodeVisitor::Decode(const char* name, double &element);
template void TomlDecodeVisitor::Decode(const char* name, std::string &element);
template <> 
inline void TomlDecodeVisitor::Decode(const char* name, uint32_t &element) {
    int64_t element1 = element;
    Decode(name, element1);
    element = element1;
}
//FIXME: possibly to overflow
template <> 
inline void TomlDecodeVisitor::Decode(const char* name, uint64_t &element) {
    int64_t element1 = element;
    Decode(name, element1);
    element = element1;
}
template <>
inline void TomlDecodeVisitor::Decode(const char* name, TomlDecodeElementI& element) {
    auto nowObj = nowObjs_.top();
    auto v = nowObj->find(name);
    if (v) {
        nowObjs_.push(v);
        element.Accept(*this);
        nowObjs_.pop();
    }
}
}
}
