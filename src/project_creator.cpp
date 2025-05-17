#include "../include/project_management/project_creator.hpp"
#include "../include/utils/utils.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace project_management {
ProjectConfig prompt_user_input(const std::string &folder) {
  ProjectConfig config;
  config.folder = folder;
  config.name = utils::input_with_prompt("Enter project name: ");
  config.language = utils::input_with_prompt("Choose language (c/cpp): ");
  config.standard =
      utils::input_with_prompt("Enter standard (e.g., c11, c++17): ");
  config.compiler =
      utils::input_with_prompt("Choose compiler (gcc, g++, clang, clang++): ");
  return config;
}

void create(const ProjectConfig &config) {
  std::filesystem::create_directories(config.folder + "/src");
  std::filesystem::create_directories(config.folder + "/include");

  std::ofstream config_file(config.folder + "/zyn.toml");
  config_file << "[project]\n";
  config_file << "version = \"1.0.0\"\n";
  config_file << "name = \"" << config.name << "\"\n";
  config_file << "language = \"" << config.language << "\"\n";
  config_file << "standard = \"" << config.standard << "\"\n";
  config_file << "compiler = \"" << config.compiler << "\"\n\n";

  config_file << "[directories]\n";
  config_file << "sources = \"src\"\n";
  config_file << "include = \"include\"\n";
  config_file << "build = \"build\"\n\n";

  config_file << "[dependencies]\n\n";

  config_file << "[libraries]\n";
  config_file << "lib_dirs = []\n";
  config_file << "libraries = []\n";

  std::string lang = config.language;
  std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);

  if (lang == "c") {
    std::ofstream main_c(config.folder + "/src/main.c");
    main_c << "#include <stdio.h>\n\n";
    main_c << "int main() {\n";
    main_c << "    printf(\"Hello World\");\n";
    main_c << "    return 0;\n";
    main_c << "}\n";
    main_c.close();
  } else if (lang == "cpp") {
    std::ofstream main_cpp(config.folder + "/src/main.cpp");
    main_cpp << "#include <iostream>\n\n";
    main_cpp << "int main() {\n";
    main_cpp << "    std::cout << \"Hello World\" << std::endl;\n";
    main_cpp << "    return 0;\n";
    main_cpp << "}\n";
    main_cpp.close();
  } else {
    std::cout << "Unknown language.\n";
  }
}
} // namespace project_management
