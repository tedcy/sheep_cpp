#pragma once
class Buffer {
public:
    Buffer() = default;
    Buffer(unsigned long size);
    ~Buffer();
    unsigned long Size();
    unsigned long Capacity();
    unsigned long Write(char *data, unsigned long offset, unsigned long length);
    unsigned long Read(char *data, unsigned long offset, unsigned long length);
private:
    char *data_ = nullptr;
    unsigned long size_ = 0;
    unsigned long capacity_ = 0;
    static const unsigned long defaultCapacity = 1024;
    unsigned long growSize(unsigned long n);
};
class BufferWrapper : public Buffer{
public:
    //for write
    //left size = size_ - offset_
    BufferWrapper(unsigned long size);
    unsigned long Write(char *data, unsigned long length);
    unsigned long LeftWrite();
    //for read
    //left size = size_ - offset_
    BufferWrapper(char *data, unsigned long length);
    unsigned long Read(char *data, unsigned long length);
    unsigned long LeftRead();
    void AddOffset(unsigned long length);
private:
    unsigned long offset_ = 0;
};
