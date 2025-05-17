#pragma once
#include <string>

namespace project_management {
struct ProjectConfig {
  std::string name;
  std::string language;
  std::string standard;
  std::string compiler;
  std::string folder;
};
void create(const ProjectConfig &config);
ProjectConfig prompt_user_input(const std::string &folder);
}; // namespace project_management
