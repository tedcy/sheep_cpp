#pragma once

namespace small_packages{
class noncopyable {
public:
    noncopyable() = default;
    virtual ~noncopyable() = default;
    noncopyable(noncopyable&) = delete;
    noncopyable& operator=(noncopyable&) = delete;
};
}
