#include "../include/project_management/parser.hpp"
#include <cstdlib>
#include <filesystem>
#include <future>
#include <iostream>

namespace project_management {
int run_command(const std::string &cmd) {
  std::cout << "Running: " << cmd << "\n";
  int ret = std::system(cmd.c_str());
  if (ret != 0) {
    std::cerr << "Command failed with code " << ret << "\n";
  }
  return ret;
}

void run(std::stringstream &compile_cmd, const std::string &profile) {
  namespace fs = std::filesystem;
  fs::create_directories(".zyn/build/");

  auto install_future = std::async(std::launch::async,
                                   []() { return run_command("zyn install"); });

  install_future.get();

  Config cfg = parse("zyn.toml");

  if (cfg.profiles.count(profile) > 0) {
    for (const auto &flag : cfg.profiles.at(profile)) {
      compile_cmd << " " << flag;
    }
  } else {
    std::cerr << "Error: Profile '" << profile
              << "' not found in zyn.toml. No compile flags applied.\n";
  }

  int compile_ret = run_command(compile_cmd.str());
  if (compile_ret != 0) {
    std::cerr << "Compilation failed, aborting run.\n";
    return;
  }

  std::string run_cmd = "./.zyn/build/" + cfg.name;
  int run_ret = run_command(run_cmd);
  if (run_ret != 0) {
    std::cerr << "Run failed with code " << run_ret << "\n";
  }
}

} // namespace project_management