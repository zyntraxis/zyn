#include "../include/project_management/ide_generator.hpp"
#include "../include/project_management/parser.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace project_management {

void generate_vscode_files() {
  const auto config = parse("zyn.toml");
  fs::create_directories(".vscode");

  json c_cpp_properties;
  c_cpp_properties["configurations"] = {{
      {"name", "Zyn"},
      {"intelliSenseMode", "gcc-x64"},
      {"compilerPath", config.compiler},
      {"cStandard", config.language == "c" ? config.standard : "c11"},
      {"cppStandard", config.language == "cpp" ? config.standard : "c++17"},
      {"includePath", {config.include, ".zyn/build", ".zyn/deps"}},
      {"defines", json::array()},
  }};
  c_cpp_properties["version"] = 4;

  std::ofstream(".vscode/c_cpp_properties.json") << c_cpp_properties.dump(2);

  json task = {{"label", "build"},
               {"type", "shell"},
               {"command", "zyn run --release"},
               {"group", {{"kind", "build"}, {"isDefault", true}}},
               {"problemMatcher", json::array({"$gcc"})}};

  json tasks;
  tasks["version"] = "2.0.0";
  tasks["tasks"] = json::array({task});

  std::ofstream(".vscode/tasks.json") << tasks.dump(2);

  json launch;
  launch["version"] = "0.2.0";
  launch["configurations"] = json::array(
      {{{"name", "Launch Zyn"},
        {"type", "cppdbg"},
        {"request", "launch"},
        {"program", "./zyn_build/" + config.name},
        {"args", json::array()},
        {"stopAtEntry", false},
        {"cwd", "${workspaceFolder}"},
        {"environment", json::array()},
        {"externalConsole", false},
        {"MIMode", "gdb"},
        {"setupCommands",
         json::array({{{"description", "Enable pretty-printing for gdb"},
                       {"text", "-enable-pretty-printing"},
                       {"ignoreFailures", true}}})}}});
  std::ofstream(".vscode/launch.json") << launch.dump(2);
}

void generate_clion_config() {
  const auto config = parse("zyn.toml");
  fs::create_directories(".clion");

  std::ofstream(".clion/CMakeLists.txt") << R"(
  cmake_minimum_required(VERSION 3.15)
  project(ZynProject)
  
  set(CMAKE_CXX_STANDARD 17)
  include_directories(
    )" << config.include << R"(
    .zyn/build
    .zyn/deps
  )
  
  add_executable(zyn_build src/main.cpp)
  )";
}

void generate_qtcreator_config() {
  const auto config = parse("zyn.toml");
  fs::create_directories(".qtcreator");

  std::ofstream(".qtcreator/ZynProject.pro") << R"(
  TEMPLATE = app
  CONFIG += console c++17
  INCLUDEPATH += )" << config.include << R"( .zyn/build .zyn/deps
  
  SOURCES += src/main.cpp
  )";
}

} // namespace project_management
