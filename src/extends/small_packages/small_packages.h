#pragma once
#include "small_strings/small_strings.h"
#include "small_lock/small_lock_factory.h"

namespace small_packages{
class noncopyable {
public:
    noncopyable() = default;
    virtual ~noncopyable() = default;
    noncopyable(noncopyable&) = delete;
    noncopyable& operator=(noncopyable&) = delete;
};
}
