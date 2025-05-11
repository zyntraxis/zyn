#include "../include/builder.hpp"
#include "../include/init.hpp"
#include "../include/parser.hpp"
#include "../include/runner.hpp"
#include <future>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  auto config_future = std::async(
      std::launch::async, [] { return parser::parse_config("config.zyn"); });

  if (argc < 2)
    return 1;

  std::string cmd = argv[1];

  if (cmd == "init") {
    std::future<void> init_task;
    if (argc == 2 || std::string(argv[2]).empty()) {
      init_task = std::async(std::launch::async, init::init_test);
    } else if (std::string(argv[2]) == "debug") {
      init_task = std::async(std::launch::async, init::init_debug);
    } else if (std::string(argv[2]) == "release") {
      init_task = std::async(std::launch::async, init::init_release);
    } else {
      std::cerr << "\033[31mUnknown init option: " << argv[2] << "\033[0m\n";
      return 1;
    }
    init_task.get();
  } else if (cmd == "add" && argc > 2) {
    std::vector<std::string> git_urls(argv + 2, argv + argc);
    auto add_task = std::async(std::launch::async, add_dependencies, git_urls,
                               "config.zyn");
    add_task.get();
  } else if (cmd == "run") {
    Config config = config_future.get();
    auto run_task = std::async(std::launch::async, runner::run, config);
    run_task.get();
  } else if (cmd == "clean") {
    auto clean_task =
        std::async(std::launch::async, clean_project, "build", "dependencies");
    clean_task.get();
  } else {
    std::cerr << "\033[31mUnknown command: " << cmd << "\033[0m\n";
    return 1;
  }

  return 0;
}