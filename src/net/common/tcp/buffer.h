#pragma once

#include <vector>
#include <cstdint>

#include "small_packages.h"

namespace sheep{
namespace net{
//Push and PopHead as queue
class Buffer: public small_packages::noncopyable{
public:
    Buffer();
//for user
    void Push(char *buf, uint64_t len);
    uint64_t PopHead(char *buf, uint64_t len);
private:
//for tcp_connection
friend class TcpConnection;
    void Write(char *buf, uint64_t len);
    uint64_t Read(char *buf, uint64_t len);
    uint64_t UpdateReadIndex(uint64_t len);

    void checkWriteSize(uint64_t len);

    uint64_t readIndex_ = 0;
    uint64_t writeIndex_ = 0;
    static const int InitSize_ = 1024;
    std::vector<char> buffer_;
};
}
}