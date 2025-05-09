#include "../include/parser.hpp"
#include <fstream>
#include <unordered_map>

namespace parser {

Config parse_config(const std::string &filename) {
  std::ifstream file(filename);
  Config config;
  std::string line, section;

  auto trim = [](std::string &s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
  };

  std::unordered_map<std::string, std::string *> field_map = {
      {"config.version", &config.version},
      {"config.project", &config.project},
      {"config.language", &config.language},
      {"config.standard", &config.standard},
      {"config.build_type", &config.build_type},
      {"config.compiler", &config.compiler},
      {"config.warnings", &config.warnings},
      {"config.optimization", &config.optimization},
      {"config.c_cache", &config.c_cache},

      {"directories.sources", &config.sources_dir},
      {"directories.include", &config.include_dir},
      {"directories.build", &config.build_dir},

      {"dependencies.", &config.dependencies},

      {"linting.linter", &config.linter},
      {"linting.config_file", &config.linter_config_file},
      {"linting.enable_checks", &config.linter_enable_checks},
      {"linting.treat_warnings_as_errors",
       &config.linter_treat_warnings_as_errors},

      {"analysis.static_analyzer", &config.static_analyzer},
      {"analysis.cppcheck_enable", &config.cppcheck_enable},
      {"analysis.cppcheck_inconclusive", &config.cppcheck_inconclusive},
      {"analysis.cppcheck_force", &config.cppcheck_force},

      {"profiling.tool", &config.profiling_tool},
  };

  while (std::getline(file, line)) {
    trim(line);
    if (line.empty() || line[0] == '#')
      continue;

    if (line.front() == '[' && line.back() == ']') {
      section = line.substr(1, line.size() - 2);
      trim(section);
    } else {
      auto eq_pos = line.find('=');
      if (eq_pos == std::string::npos)
        continue;

      std::string key = line.substr(0, eq_pos);
      std::string value = line.substr(eq_pos + 1);
      trim(key);
      trim(value);

      std::string full_key = section + "." + key;
      auto it = field_map.find(full_key);
      if (it != field_map.end()) {
        *(it->second) = value;
      }
    }
  }

  return config;
}

} // namespace parser
