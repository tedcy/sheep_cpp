#pragma once

#include <memory>

#include "small_config.h"

namespace small_config{
std::shared_ptr<ConfigI> MakeConfig();
}
