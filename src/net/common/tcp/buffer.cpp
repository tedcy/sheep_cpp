#include "buffer.h"
#include "log.h"

namespace sheep{
namespace net{
Buffer::Buffer() :
    buffer_(InitSize_){
}

void Buffer::Reset() {
    readIndex_ = 0;
    writeIndex_ = 0;
    std::vector<char> tmp;
    buffer_.swap(tmp);
}
    
void Buffer::Push(const char *buf, uint64_t len) {
    Write(buf, len);
}
uint64_t Buffer::PopHead(char *buf, uint64_t len) {
    len = Read(buf, len);
    len = UpdateReadIndex(len);
    return len;
}
void Buffer::Write(const char *buf, uint64_t len) {
    checkWriteSize(len);
    buffer_.insert(buffer_.begin() + writeIndex_, 
            buf, buf + len);
    writeIndex_ += len;
}
uint64_t Buffer::Read(char *buf, uint64_t len) {
    //LOG(DEBUG) << readIndex_<< " " << writeIndex_;
    //readIndex can be <= writeIndex
    if (readIndex_ == writeIndex_) {
        return 0;
    }
    //len must be > 0
    if (readIndex_ + len > writeIndex_) {
        len = writeIndex_ - readIndex_;
    }
    std::copy(buffer_.begin() + readIndex_,
            buffer_.begin() + readIndex_ + len, buf);
    return len;
}
uint64_t Buffer::UpdateReadIndex(uint64_t len) {
    readIndex_ += len;
    //read finish, reset indexs
    if (readIndex_ == writeIndex_) {
        Reset();
    }
    return len;
}
void Buffer::checkWriteSize(uint64_t len) {
    //TODO delete unuse readData if need reverse
}
}
}
