#include "../include/init.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace init {
void init_debug() {
  auto project = std::filesystem::current_path().filename().string();
  std::string lang, standard, compiler;

  std::cout << "Language (c/cpp) -> ";
  std::getline(std::cin, lang);

  std::cout << "Standard -> ";
  std::getline(std::cin, standard);

  std::cout << "Compiler (clang++/clang) -> ";
  std::getline(std::cin, compiler);

  std::ofstream("config.zyn")
      << "[config]\n"
      << "version=1.0.0\n"
      << "project=" << project << "\nlanguage=" << lang
      << "\nstandard=" << standard << "\nbuild_type=debug"
      << "\ncompiler=" << compiler
      << "\n\n[directories]\nsources=src\ninclude=include\nbuild=build\n\n["
         "dependencies]\n\n[linting]\nlinter=clang-tidy\nconfig_file=.clang-"
         "tidy\nenable_checks=all\ntreat_warnings_as_errors=true\n\n[analysis]"
         "\nstatic_analyzer=cppcheck\ncppcheck_enable=all\ncppcheck_"
         "inconclusive=true\ncppcheck_force=true\n\n[profiling]\ntool=valgrind";

  std::filesystem::path src_dir = "src";
  std::filesystem::create_directory(src_dir);

  std::filesystem::path include_dir = "include";
  std::filesystem::create_directory(include_dir);

  if (lang == "c") {
    std::ofstream("src/main.c")
        << "#include <stdio.h>\n\nint main() {\n\tprintf("
           "\"Let's start :)\");\n\treturn 0;\n}";
  } else if (lang == "cpp") {
    std::ofstream("src/main.cpp")
        << "#include <iostream>\n\nint main() {\n\tstd::cout << "
           "\"Let's start :)\" << std::endl;\n\treturn 0;\n}";
  }
}

void init_release() {
  auto project = std::filesystem::current_path().filename().string();
  std::string lang, standard, compiler;

  std::cout << "Language (c/cpp) -> ";
  std::getline(std::cin, lang);

  std::cout << "Standard -> ";
  std::getline(std::cin, standard);

  std::cout << "Compiler (clang++/clang) -> ";
  std::getline(std::cin, compiler);

  std::ofstream("config.zyn")
      << "[config]\n"
      << "version=1.0.0\n"
      << "project=" << project << "\nlanguage=" << lang
      << "\nstandard=" << standard << "\nbuild_type=release"
      << "\ncompiler=" << compiler << "\nwarnings=none" << "\noptimization=max"
      << "\nc_cache=on"
      << "\n\n[directories]\nsources=src\ninclude=include\nbuild=build\n\n["
         "dependencies]";

  std::filesystem::path src_dir = "src";
  std::filesystem::create_directory(src_dir);

  std::filesystem::path include_dir = "include";
  std::filesystem::create_directory(include_dir);

  if (lang == "c") {
    std::ofstream("src/main.c")
        << "#include <stdio.h>\n\nint main() {\n\tprintf("
           "\"Let's start :)\");\n\treturn 0;\n}";
  } else if (lang == "cpp") {
    std::ofstream("src/main.cpp")
        << "#include <iostream>\n\nint main() {\n\tstd::cout << "
           "\"Let's start :)\" << std::endl;\n\treturn 0;\n}";
  }
}

void init_test() {
  auto project = std::filesystem::current_path().filename().string();
  std::string lang, standard, compiler;

  std::cout << "Language (c/cpp) -> ";
  std::getline(std::cin, lang);

  std::cout << "Standard -> ";
  std::getline(std::cin, standard);

  std::cout << "Compiler (clang++/clang) -> ";
  std::getline(std::cin, compiler);

  std::ofstream("config.zyn")
      << "[config]\n"
      << "version=1.0.0\n"
      << "project=" << project << "\nlanguage=" << lang
      << "\nstandard=" << standard << "\nbuild_type=test"
      << "\ncompiler=" << compiler << "\nwarnings=all"
      << "\noptimization=balanced"
      << "\nc_cache=on"
      << "\n\n[directories]\nsources=src\ninclude=include\nbuild=build\n\n["
         "dependencies]";

  std::filesystem::path src_dir = "src";
  std::filesystem::create_directory(src_dir);

  std::filesystem::path include_dir = "include";
  std::filesystem::create_directory(include_dir);

  if (lang == "c") {
    std::ofstream("src/main.c")
        << "#include <stdio.h>\n\nint main() {\n\tprintf("
           "\"Let's start :)\");\n\treturn 0;\n}";
  } else if (lang == "cpp") {
    std::ofstream("src/main.cpp")
        << "#include <iostream>\n\nint main() {\n\tstd::cout << "
           "\"Let's start :)\" << std::endl;\n\treturn 0;\n}";
  }
}
} // namespace init