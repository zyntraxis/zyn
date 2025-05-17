#include "../include/project_management/clean_project.hpp"
#include <iostream>

namespace project_management {
void clean_project(const fs::path &folder) {
  try {
    if (fs::remove_all(folder) > 0) {
      std::cout << "The project has been cleared.\n";
    } else {
      std::cout << "The project has already been cleared.\n";
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error while cleaning the project.\n";
  }
}
} // namespace project_management