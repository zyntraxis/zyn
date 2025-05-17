#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace project_management {
void clean_project(const fs::path &folder);
}