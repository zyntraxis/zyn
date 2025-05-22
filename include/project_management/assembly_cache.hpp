#pragma once

#include "parser.hpp"
#include <string>

namespace project_management {
std::string hash_source_files(const Config &config);
bool needs_rebuild(const Config &config);
void update_cache(const Config &config);
} // namespace project_management