#include "../include/project_management/parser.hpp"
#include "../external/tomlplusplus/toml.hpp"
#include <fstream>
#include <map>
#include <sstream>

namespace project_management {
Config parse(std::string config_file) {
  toml::table tbl = toml::parse_file(config_file);

  Config config;

  config.version = tbl["project"]["version"].value_or("0.0.0");
  config.name = tbl["project"]["name"].value_or("default_name");
  config.language = tbl["project"]["language"].value_or("C++");
  config.standard = tbl["project"]["standard"].value_or("C++17");
  config.compiler = tbl["project"]["compiler"].value_or("g++");
  config.sources = tbl["directories"].value_or("src");
  config.include = tbl["directories"].value_or("include");
  config.build = tbl["directories"].value_or("build");

  if (auto dep_table = tbl["dependencies"].as_table()) {
    for (auto &[key, val] : *dep_table) {
      if (auto dep = val.as_table()) {
        Dependency dependency;
        dependency.git =
            dep->contains("git") ? dep->at("git").value_or("") : "";
        dependency.tag =
            dep->contains("tag") ? dep->at("tag").value_or("") : "";
        dependency.path =
            dep->contains("path") ? dep->at("path").value_or("") : "";

        config.dependencies[std::string(key.str())] = dependency;
      }
    }
  }

  if (auto libs_table = tbl["libraries"].as_table()) {
    if (auto libs_array = (*libs_table)["lib_dirs"].as_array()) {
      for (auto &lib : *libs_array) {
        if (lib.is_string()) {
          config.lib_dirs.push_back(lib.value_or(""));
        }
      }
    }
  }

  if (auto libs_table = tbl["libraries"].as_table()) {
    if (auto libs_array = (*libs_table)["libraries"].as_array()) {
      for (auto &lib : *libs_array) {
        if (lib.is_string()) {
          config.libraries.push_back(lib.value_or(""));
        }
      }
    }
  }

  return config;
}

void save(const std::string &path, const Config &config) {
  std::ifstream in(path);
  std::stringstream buffer;
  bool in_dependencies = false;
  bool dependencies_written = false;
  std::map<std::string, bool> written;

  for (const auto &[name, _] : config.dependencies)
    written[name] = false;

  std::string line;
  while (std::getline(in, line)) {
    if (line == "[dependencies]") {
      in_dependencies = true;
      buffer << line << "\n";
      for (const auto &[name, dep] : config.dependencies) {
        buffer << name << " = { ";
        if (!dep.git.empty())
          buffer << "git = \"" << dep.git << "\"";
        if (!dep.tag.empty()) {
          if (!dep.git.empty())
            buffer << ", ";
          buffer << "tag = \"" << dep.tag << "\"";
        }
        if (!dep.path.empty()) {
          if (!dep.git.empty() || !dep.tag.empty())
            buffer << ", ";
          buffer << "path = \"" << dep.path << "\"";
        }
        buffer << " }\n";
        written[name] = true;
      }
      dependencies_written = true;

      while (std::getline(in, line)) {
        if (line.empty() || line[0] == '[') {
          buffer << line << "\n";
          if (!line.empty() && line[0] == '[')
            in_dependencies = false;
          break;
        }
      }
    } else {
      buffer << line << "\n";
    }
  }

  in.close();

  if (!dependencies_written) {
    buffer << "\n[dependencies]\n";
    for (const auto &[name, dep] : config.dependencies) {
      if (!written[name]) {
        buffer << name << " = { ";
        if (!dep.git.empty())
          buffer << "git = \"" << dep.git << "\"";
        if (!dep.tag.empty()) {
          if (!dep.git.empty())
            buffer << ", ";
          buffer << "tag = \"" << dep.tag << "\"";
        }
        if (!dep.path.empty()) {
          if (!dep.git.empty() || !dep.tag.empty())
            buffer << ", ";
          buffer << "path = \"" << dep.path << "\"";
        }
        buffer << " }\n";
      }
    }
  }

  std::ofstream out(path);
  out << buffer.str();
}
} // namespace project_management