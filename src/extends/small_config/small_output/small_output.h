#pragma once

#include "small_config_interface.h"
#include <iostream>

namespace small_config{

namespace internal{
class OutputEncodeVisitor;
using OutputEncodeElementI = EncodeElementI<OutputEncodeVisitor>;
class OutputEncodeVisitor {
public:
    OutputEncodeVisitor() {
    }
    template <typename T>
    void Encode(const char* name, T &element) {
    }
    void GetResult(std::string &out) {
    }
private:
};
template void OutputEncodeVisitor::Encode(const char* name, int32_t &element);
template void OutputEncodeVisitor::Encode(const char* name, int64_t &element);
template void OutputEncodeVisitor::Encode(const char* name, bool &element);
template void OutputEncodeVisitor::Encode(const char* name, double &element);
template void OutputEncodeVisitor::Encode(const char* name, std::string& element);
template void OutputEncodeVisitor::Encode(const char* name, uint32_t &element);
template void OutputEncodeVisitor::Encode(const char* name, uint64_t &element);
template void OutputEncodeVisitor::Encode(const char* name, OutputEncodeElementI& element);

class OutputDecodeVisitor;
using OutputDecodeElementI = DecodeElementI<OutputDecodeVisitor>;
class OutputDecodeVisitor {
public:
    explicit OutputDecodeVisitor(const std::string &in) {
    }
    void Parse(std::string &errMsg) {
    }
    template <typename T>
    void Decode(const char* name, T &element) {
        std::cout << "key: " << name << " value: " << element << std::endl;
    }
private:
};
template void OutputDecodeVisitor::Decode(const char* name, int32_t &element);
template void OutputDecodeVisitor::Decode(const char* name, int64_t &element);
template void OutputDecodeVisitor::Decode(const char* name, bool &element);
template void OutputDecodeVisitor::Decode(const char* name, double &element);
template void OutputDecodeVisitor::Decode(const char* name, std::string &element);
template void OutputDecodeVisitor::Decode(const char* name, uint32_t &element);
template void OutputDecodeVisitor::Decode(const char* name, uint64_t &element);
template <> 
inline void OutputDecodeVisitor::Decode(const char* name, OutputDecodeElementI& element) {
    std::cout << "enter " << name << std::endl;
}
}
}
