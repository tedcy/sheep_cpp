#include "buffer.h"
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <iostream>
using namespace std;

Buffer::Buffer(unsigned long size) {
    capacity_ = growSize(size);
    size_ = size;
    auto new_ptr = static_cast<char*>(std::realloc(data_, capacity_));
    assert(new_ptr != nullptr);
    data_ = new_ptr;
}

Buffer::~Buffer() {
    if (data_ != nullptr) {
        free(data_);
    }
}

unsigned long Buffer::Size() {
    return size_;
}

unsigned long Buffer::Capacity() {
    return capacity_;
}

//todo malloc异常
unsigned long Buffer::Write(char *data, unsigned long offset, unsigned long length) {
    if (offset + length > size_) {
        size_ = offset + length;
    }
    if (offset + length > capacity_) {
        capacity_ = growSize(offset + length);
        data_ = static_cast<char*>(std::realloc(data_, capacity_));
    }
    std::memcpy(data_ + offset, data, length);
    return length;
}

unsigned long Buffer::Read(char *data, unsigned long offset, unsigned long length) {
    unsigned long readLength = 0;
    if (offset >= size_) {
        return readLength;
    }
    if (length > size_ - offset) {
        readLength = size_ - offset;
    }
    std::memcpy(data, data_ + offset, readLength);
    return readLength;
}

unsigned long Buffer::growSize(unsigned long n) {
    unsigned long needSize;
    if (capacity_ != 0) {
        needSize = capacity_;
    }else {
        needSize = defaultCapacity;
    }
    for (;;) {
        if (needSize >= n) {
            break;
        }
        needSize << 1;
    }
    return needSize;
}

BufferWrapper::BufferWrapper(unsigned long size):
    Buffer(size) {
}

unsigned long BufferWrapper::Write(char *data, unsigned long length) {
    auto n = Buffer::Write(data, offset_, length);
    offset_ += n;
    return n;
}

unsigned long BufferWrapper::LeftWrite() {
    return Size() - offset_;
}

//不能直接用write方法，因为会追加offset
BufferWrapper::BufferWrapper(char *data, unsigned long length) :
    Buffer() {
    auto n = Buffer::Write(data, offset_, length);
}

unsigned long BufferWrapper::Read(char *data, unsigned long length) {
    auto n = Buffer::Read(data, offset_, length);
    return n;
}

unsigned long BufferWrapper::LeftRead() {
    return Size() - offset_;
}

void BufferWrapper::AddOffset(unsigned long length) {
    offset_ += length;
    return;
}
