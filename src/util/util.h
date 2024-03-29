#pragma once

#include <chrono>
#include <ctime>

inline uint64_t UnixTimeSecond() {
    std::chrono::time_point<std::chrono::system_clock,std::chrono::seconds> tp =
        std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return uint64_t(timestamp);
}
inline uint64_t UnixTimeMilliSecond() {
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return uint64_t(timestamp);
}