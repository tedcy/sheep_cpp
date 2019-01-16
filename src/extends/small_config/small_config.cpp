#include "small_config.h"
#include "toml_config.h"

namespace small_config{
std::shared_ptr<ConfigI> MakeConfig() {
    return std::make_shared<TomlConfig>();
}
}
