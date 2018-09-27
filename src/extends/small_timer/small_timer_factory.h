#pragma once
#include "small_timer.h"
#include <memory>

namespace small_timer{
std::shared_ptr<TimerI> MakeTimer();
uint64_t UnixTimeSecond();
uint64_t UnixTimeMilliSecond();
}//namespace small_timer
