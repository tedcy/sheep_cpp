#pragma once

#include <string>
#include <map>

namespace small_config{
/*
class VisitorI{
public:
    void Encode(const char* name, ElementI& element);
    void Encode(const char* name, uint64_t element);
    void Encode(const char* name, int64_t element);
    void Encode(const char* name, uint32_t element);
    void Encode(const char* name, int32_t element);
    void Encode(const char* name, bool element);
    void Encode(const char* name, std::vector<T> &element);
};*/
namespace internal{
template<typename Visitor>
class EncodeElementI{
public:
    virtual ~EncodeElementI() = default;
    virtual void Accept(Visitor &v) = 0;
};
template<typename Visitor>
class DecodeElementI{
public:
    virtual ~DecodeElementI() = default;
    virtual void Accept(Visitor &v) = 0;
};
}
}
