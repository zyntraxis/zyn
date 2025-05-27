#include "../include/dependency_manager/git_dependency.hpp"
#include "../include/dependency_manager/local_dependency.hpp"
#include "../include/project_management/clean_project.hpp"
#include "../include/project_management/compile_cmd_generator.hpp"
#include "../include/project_management/ide_generator.hpp"
#include "../include/project_management/project_creator.hpp"
#include "../include/project_management/runner.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <command> [arguments]\n";
    return 1;
  }

  std::string command = argv[1];

  try {
    if (command == "new") {
      if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " new <folder>\n";
        return 1;
      }
      auto config = project_management::prompt_user_input(argv[2]);
      project_management::create(config);
      std::cout << "Project \"" << config.name << "\" created.\n";

    } else if (command == "install") {
      if (argc == 3) {
        dependency_manager::install_from_url(argv[2]);
      } else {
        dependency_manager::install_all_from_config();
      }

    } else if (command == "add") {
      if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " add <local_lib_path>\n";
        return 1;
      }
      dependency_manager::add_local_dependency(argv[2]);

    } else if (command == "run") {
      std::stringstream cmd = project_management::generate_compile_cmd();

      if (argc == 3) {
        project_management::run(cmd, argv[2]);
      } else {
        project_management::run(cmd, "--test");
      }

    } else if (command == "clean") {
      fs::path zyn_folder = fs::current_path() / ".zyn";
      project_management::clean_project(zyn_folder);

    } else if (command == "update") {
      dependency_manager::update_all_dependencies();
    } else if (command == "ide") {
      if (std::string(argv[2]) == "--vscode") {
        project_management::generate_vscode_files();
        std::cout << "VSCode config generated in .vscode/\n";
      } else if (std::string(argv[2]) == "--clion") {
        project_management::generate_clion_config();
        std::cout << "CLion config generated in .clion/\n";
      } else if (std::string(argv[2]) == "--qtcreator") {
        project_management::generate_qtcreator_config();
        std::cout << "Qtcreator config generated in .qtcreator/\n";
      }
    } else {
      std::cerr << "Unknown command: " << command << "\n";
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "[Zyn] Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
