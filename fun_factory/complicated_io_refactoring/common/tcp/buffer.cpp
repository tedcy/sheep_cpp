#include "buffer.h"
#include "log.h"

Buffer::Buffer() :
    buffer_(InitSize_){
}
    
void Buffer::Push(char *buf, uint64_t len) {
    Write(buf, len);
}
uint64_t Buffer::PopHead(char *buf, uint64_t len) {
    len = Read(buf, len);
    len = UpdateReadIndex(len);
    return len;
}
void Buffer::Write(char *buf, uint64_t len) {
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
        readIndex_ = 0;
        writeIndex_ = 0;
    }
    return len;
}
void Buffer::checkWriteSize(uint64_t len) {
    //TODO delete unuse readData if need reverse
}
