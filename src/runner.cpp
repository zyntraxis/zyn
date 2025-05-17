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
} // namespace run_command(conststd::string

void run(std::stringstream &compile_cmd, const std::string &type) {
  namespace fs = std::filesystem;
  fs::create_directories(".zyn/build/");

  auto install_future = std::async(std::launch::async,
                                   []() { return run_command("zyn install"); });

  install_future.get();

  if (type == "--release") {
    compile_cmd << " -w -O3 -ffast-math -finline-functions -funroll-loops "
                   "-fomit-frame-pointer -march=native -flto "
                   "-DNDEBUG -fstrict-aliasing "
                   "-fmerge-all-constants";
  } else if (type == "--debug") {
    compile_cmd << " -g -O0 -DDEBUG -fno-inline -fno-omit-frame-pointer "
                   "-fsanitize=address -fsanitize=undefined";
  }

  int compile_ret = run_command(compile_cmd.str());
  if (compile_ret != 0) {
    std::cerr << "Compilation failed, aborting run.\n";
    return;
  }

  Config cfg = parse("zyn.toml");

  std::string run_cmd = "./.zyn/build/" + cfg.name;
  int run_ret = run_command(run_cmd);
  if (run_ret != 0) {
    std::cerr << "Run failed with code " << run_ret << "\n";
  }
}
} // namespace project_management