#include "../include/init.hpp"
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>

namespace init {
namespace fs = std::filesystem;

std::mutex io_mutex;

std::string get_input(const std::string &prompt) {
  std::string input;
  {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout << prompt;
    std::getline(std::cin, input);
  }
  return input;
}

void write_config_file(const std::string &project, const std::string &lang,
                       const std::string &standard, const std::string &compiler,
                       const std::string &build_type,
                       const std::string &extra = "") {
  std::ofstream config("config.zyn");
  config << "[config]\n"
         << "version=1.0.0\n"
         << "project=" << project << "\nlanguage=" << lang
         << "\nstandard=" << standard << "\nbuild_type=" << build_type
         << "\ncompiler=" << compiler;
  if (!extra.empty())
    config << extra;

  config << "\n\n[directories]\nsources=src\ninclude=include\nbuild=build\n\n["
            "dependencies]";
}

void create_main_file(const std::string &lang) {
  fs::create_directory("src");
  if (lang == "c") {
    std::ofstream("src/main.c")
        << "#include <stdio.h>\n\nint main() {\n\tprintf(\"Let's start "
           ":)\");\n\treturn 0;\n}";
  } else if (lang == "cpp") {
    std::ofstream("src/main.cpp")
        << "#include <iostream>\n\nint main() {\n\tstd::cout << \"Let's start "
           ":)\" << std::endl;\n\treturn 0;\n}";
  }
}

void async_init(const std::string &build_type, const std::string &extra = "") {
  auto project = fs::current_path().filename().string();
  std::string lang = get_input("Language (c/cpp) -> ");
  std::string standard = get_input("Standard -> ");
  std::string compiler = get_input("Compiler (clang++/clang) -> ");

  auto config_task = std::async(std::launch::async, write_config_file, project,
                                lang, standard, compiler, build_type, extra);

  auto create_dirs = std::async(std::launch::async, []() {
    fs::create_directory("src");
    fs::create_directory("include");
  });

  auto main_file = std::async(std::launch::async, create_main_file, lang);

  config_task.get();
  create_dirs.get();
  main_file.get();
}

void init_debug() {
  std::string extra =
      "\n\n[linting]\nlinter=clang-tidy\nconfig_file=.clang-tidy\nenable_"
      "checks=all\ntreat_warnings_as_errors=true"
      "\n\n[analysis]\nstatic_analyzer=cppcheck\ncppcheck_enable=all\ncppcheck_"
      "inconclusive=true\ncppcheck_force=true"
      "\n\n[profiling]\ntool=valgrind";
  async_init("debug", extra);
}

void init_release() {
  std::string extra = "\nwarnings=none\noptimization=max\nc_cache=on";
  async_init("release", extra);
}

void init_test() {
  std::string extra = "\nwarnings=all\noptimization=balanced\nc_cache=on";
  async_init("test", extra);
}

} // namespace init
