#pragma once

#include <sstream>
#include <string>

namespace project_management {
int run_command(const std::string &cmd);
void run(std::stringstream &compile_cmd, const std::string &type);
} // namespace project_management