#pragma once
#include "small_timer.h"
#include <memory>

namespace small_timer{
std::shared_ptr<TimerI> MakeTimer();
uint64_t UnixTimeSecond();
}//namespace small_timer
