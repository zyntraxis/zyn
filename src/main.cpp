#include "../include/init.hpp"
#include "../include/parser.hpp"
#include "../include/runner.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  Config config = parser::parse_config("config.zyn");

  if (argc < 2)
    return 1;

  std::string cmd = argv[1];

  if (cmd == "init") {
    if (argc == 2 || std::string(argv[2]).empty()) {
      init::init_test();
    } else if (std::string(argv[2]) == "debug") {
      init::init_debug();
    } else if (std::string(argv[2]) == "release") {
      init::init_release();
    } else {
      std::cerr << "Unknown init option: " << argv[2] << "\n";
    }
  }

  if (cmd == "run") {
    runner::run(config);
  }

  if (cmd == "show") {
    std::cout << "version = " << config.version << '\n';
    std::cout << "project = " << config.project << '\n';
    std::cout << "language = " << config.language << '\n';
    std::cout << "standard = " << config.standard << '\n';
    std::cout << "build_type = " << config.build_type << '\n';
    std::cout << "compiler = " << config.compiler << '\n';
    std::cout << "warnings = " << config.warnings << '\n';
    std::cout << "optimization = " << config.optimization << '\n';
    std::cout << "c_cache = " << config.c_cache << '\n';

    std::cout << "sources_dir = " << config.sources_dir << '\n';
    std::cout << "include_dir = " << config.include_dir << '\n';
    std::cout << "build_dir = " << config.build_dir << '\n';

    std::cout << "dependencies = " << config.dependencies << '\n';

    std::cout << "linter = " << config.linter << '\n';
    std::cout << "linter_config_file = " << config.linter_config_file << '\n';
    std::cout << "linter_enable_checks = " << config.linter_enable_checks
              << '\n';
    std::cout << "linter_treat_warnings_as_errors = "
              << config.linter_treat_warnings_as_errors << '\n';

    std::cout << "static_analyzer = " << config.static_analyzer << '\n';
    std::cout << "cppcheck_enable = " << config.cppcheck_enable << '\n';
    std::cout << "cppcheck_inconclusive = " << config.cppcheck_inconclusive
              << '\n';
    std::cout << "cppcheck_force = " << config.cppcheck_force << '\n';

    std::cout << "profiling_tool = " << config.profiling_tool << '\n';
  }

  return 0;
}
