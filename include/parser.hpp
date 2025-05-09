#pragma once

#include "config.hpp"
#include <string>

namespace parser {
Config parse_config(const std::string &filename);
}
