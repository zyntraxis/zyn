#include "../include/dependency_manager/local_dependency.hpp"
#include "../external/tomlplusplus/toml.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace dependency_manager {
void add_local_dependency(const std::string &lib_path_str) {
  const fs::path lib_path{lib_path_str};

  if (!fs::exists("zyn.toml")) {
    throw std::runtime_error("zyn.toml not found in current directory.");
  }

  const std::string lib_name = lib_path.filename().string();

  toml::table zyn;
  try {
    zyn = toml::parse_file("zyn.toml");
  } catch (const toml::parse_error &e) {
    throw std::runtime_error(std::string("Failed to parse zyn.toml: ") +
                             e.what());
  }

  if (!zyn.contains("dependencies") || !zyn["dependencies"].is_table()) {
    zyn.insert("dependencies", toml::table{});
  }

  auto &deps = *zyn["dependencies"].as_table();

  toml::table dep_table;
  dep_table.insert("path", lib_path_str);
  dep_table.is_inline(true);

  deps.insert_or_assign(lib_name, dep_table);

  std::ofstream out("zyn.toml", std::ios::trunc);
  if (!out) {
    throw std::runtime_error("Failed to open zyn.toml for writing");
  }
  out << zyn;

  std::cout << "Added local dependency \"" << lib_name << "\" to zyn.toml.\n";
}
} // namespace dependency_manager