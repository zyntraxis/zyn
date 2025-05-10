#include "../include/runner.hpp"
#include "../include/builder.hpp"
#include "../include/config.hpp"
#include <cctype>
#include <cstdlib>
#include <git2.h>
#include <iostream>
#include <sstream>
#include <string>

#include <filesystem>
#include <iostream>

namespace runner {
void run(const Config &config) {
  fs::create_directory("dependencies");
  fs::create_directory(config.build_dir);

  process_dependencies(config.dependencies);

  std::string lang = config.language == "c" ? "c" : "c++";

  std::stringstream cmd;
  cmd << config.compiler << " -std=" << lang << config.standard << " ";

  auto includes = find_include_dirs("dependencies");

  for (const auto &path : includes) {
    cmd << "-I" << path << " ";
  }

  cmd << config.sources_dir << "/*." << config.language << " -o "
      << config.build_dir << "/" << config.project;

  if (config.build_type == "release") {
    cmd << " -O3 -DNDEBUG";
  } else {
    cmd << " -g -O0";
  }

  std::cout << "[build] Running: " << cmd.str() << "\n";
  if (std::system(cmd.str().c_str()) != 0) {
    std::cerr << "[error] Compilation failed\n";
    return;
  }

  std::string exec_cmd = config.build_dir + "/" + config.project;
  std::cout << "[run] Executing: " << exec_cmd << "\n";
  std::system(exec_cmd.c_str());
}
} // namespace runner
