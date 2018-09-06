#pragma once
#include "small_timer.h"
#include <memory>

namespace small_timer{
std::shared_ptr<TimerI> MakeTimer();
void Init();
}//namespace small_timer
