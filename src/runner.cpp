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

    cmd << " -O3 -DNDEBUG -flto -march=native -funroll-loops "
           "-fomit-frame-pointer -fno-exceptions -fno-rtti -finline-functions "
           "-fprefetch-loop-arrays -Wl,--gc-sections -s -fvisibility=hidden ";
  } else {
    cmd << " -g -O0";
    std::cout << "[debug] Debug build: running additional checks\n";

    if (config.linting.enable_checks) {
      std::stringstream lint_cmd;
      lint_cmd << config.linting.linter << " "
               << "-p=" << config.build_dir << " "
               << "-config=" << config.linting.config_file << " "
               << config.sources_dir;
      if (config.linting.treat_warnings_as_errors) {
        lint_cmd << " -warnings-as-errors=*";
      }
      std::cout << "[lint] Running: " << lint_cmd.str() << "\n";
      std::system(lint_cmd.str().c_str());
    }

    if (config.analysis.cppcheck_enable) {
      std::stringstream cppcheck_cmd;
      cppcheck_cmd << config.analysis.static_analyzer << " "
                   << "--enable=all ";
      if (config.analysis.cppcheck_inconclusive)
        cppcheck_cmd << "--inconclusive ";
      if (config.analysis.cppcheck_force)
        cppcheck_cmd << "--force ";
      cppcheck_cmd << config.sources_dir;
      std::cout << "[analysis] Running: " << cppcheck_cmd.str() << "\n";
      std::system(cppcheck_cmd.str().c_str());
    }
  }

  std::cout << "[build] Compiling with: " << cmd.str() << "\n";
  int result = std::system(cmd.str().c_str());
  if (result != 0) {
    std::cerr << "[error] Compilation failed with code " << result << "\n";
    return;
  }

  std::string exec_cmd = config.build_dir + "/" + config.project;

  if (config.build_type != "release" && !config.profiling.tool.empty()) {
    std::stringstream prof_cmd;
    prof_cmd << config.profiling.tool << " " << exec_cmd;
    std::cout << "[profiling] Running: " << prof_cmd.str() << "\n";
    std::system(prof_cmd.str().c_str());
  } else {
    std::cout << "[run] Executing: " << exec_cmd << "\n";
    std::system(exec_cmd.c_str());
  }
}
} // namespace runner
