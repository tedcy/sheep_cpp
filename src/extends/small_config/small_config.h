#pragma once

#include <memory>

#include "small_config_interface.h"
#include "config_helper.h"

namespace small_config{
std::shared_ptr<ConfigI> MakeConfig();
}
