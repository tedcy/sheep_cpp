#pragma once

#include <memory>

#include "small_toml/small_toml.h"
#include "small_json/small_json.h"
#include "small_output/small_output.h"

namespace small_config{

namespace internal{
template <typename Visitor>
void Serialize(std::string& error, EncodeElementI<Visitor> &element, std::string& out) {
    Visitor visitor;
    try {
        element.Accept(visitor);
    }
    catch(const std::string& err) {
        error = err;
        return;
    }
    visitor.GetResult(out);
}
template <typename Visitor>
void UnSerialize(std::string& error, DecodeElementI<Visitor> &element, const std::string& in) {
    Visitor visitor(in);
    visitor.Parse(error);
    if (!error.empty()) {
        return;
    }
    try {
        element.Accept(visitor);
    }
    catch(const std::string& err) {
        error = err;
        return;
    }
    return;
}

inline void File2String(std::string &errMsg, const std::string &name, std::string &out) {
    std::ifstream file(name);
    if (!file) {
        errMsg = "open file " + name + " failed";
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    out = ss.str();
}
inline void String2File(std::string &errMsg, const std::string &in, const std::string &name) {
    std::ofstream file(name);
    if (!file) {
        errMsg = "open file " + name + " failed";
        return;
    }
    std::stringstream ss(in);
    file << ss.str();
}
}

using internal::JsonEncodeVisitor;
using internal::JsonDecodeVisitor;
class JsonClient: public internal::JsonEncodeElementI,
    public internal::JsonDecodeElementI {
public:
    void SerializeJson(std::string& error, std::string& out) {
        internal::Serialize<JsonEncodeVisitor>(error, *this, out);
    }
    void UnSerializeJson(std::string& error, const std::string& in) {
        internal::UnSerialize<JsonDecodeVisitor>(error, *this, in);
    }
    void SerializeJsonToFile(std::string& error, const std::string& name) {
        std::string in;
        internal::Serialize<JsonEncodeVisitor>(error, *this, in);
        internal::String2File(error, in, name);
    }
    void UnSerializeJsonFromFile(std::string& error, const std::string& name) {
        std::string out;
        internal::File2String(error, name, out);
        internal::UnSerialize<JsonDecodeVisitor>(error, *this, out);
    }
};

using internal::TomlEncodeVisitor;
using internal::TomlDecodeVisitor;
class TomlClient: public internal::TomlEncodeElementI,
    public internal::TomlDecodeElementI {
public:
    void SerializeToml(std::string& error, std::string& out) {
        internal::Serialize<TomlEncodeVisitor>(error, *this, out);
    }
    void UnSerializeToml(std::string& error, const std::string& in) {
        internal::UnSerialize<TomlDecodeVisitor>(error, *this, in);
    }
    void SerializeTomlToFile(std::string& error, const std::string& name) {
        std::string in;
        internal::Serialize<TomlEncodeVisitor>(error, *this, in);
        internal::String2File(error, in, name);
    }
    void UnSerializeTomlFromFile(std::string& error, const std::string& name) {
        std::string out;
        internal::File2String(error, name, out);
        internal::UnSerialize<TomlDecodeVisitor>(error, *this, out);
    }
};

using internal::OutputEncodeVisitor;
using internal::OutputDecodeVisitor;
class OutputClient: public internal::OutputEncodeElementI,
    public internal::OutputDecodeElementI {
public:
    void SerializeOutput(std::string& error, std::string& out) {
        internal::Serialize<OutputEncodeVisitor>(error, *this, out);
    }
    void UnSerializeOutput(std::string& error, const std::string& in) {
        internal::UnSerialize<OutputDecodeVisitor>(error, *this, in);
    }
};
}
