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

  return 0;
}
