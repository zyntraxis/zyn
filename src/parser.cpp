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

  std::unordered_map<std::string, std::string *> str_fields = {
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

      {"linting.linter", &config.linting.linter},
      {"linting.config_file", &config.linting.config_file},

      {"analysis.static_analyzer", &config.analysis.static_analyzer},

      {"profiling.tool", &config.profiling.tool},
  };

  std::unordered_map<std::string, bool *> bool_fields = {
      {"linting.enable_checks", &config.linting.enable_checks},
      {"linting.treat_warnings_as_errors",
       &config.linting.treat_warnings_as_errors},

      {"analysis.cppcheck_enable", &config.analysis.cppcheck_enable},
      {"analysis.cppcheck_inconclusive",
       &config.analysis.cppcheck_inconclusive},
      {"analysis.cppcheck_force", &config.analysis.cppcheck_force},
  };

  while (std::getline(file, line)) {
    trim(line);
    if (line.empty() || line[0] == '#')
      continue;

    if (line.front() == '[' && line.back() == ']') {
      section = line.substr(1, line.size() - 2);
      trim(section);
      continue;
    }

    if (section == "dependencies") {
      config.dependencies.push_back(line);
      continue;
    }

    auto eq_pos = line.find('=');
    if (eq_pos == std::string::npos)
      continue;

    std::string key = line.substr(0, eq_pos);
    std::string value = line.substr(eq_pos + 1);
    trim(key);
    trim(value);

    std::string full_key = section + "." + key;

    if (str_fields.count(full_key)) {
      *str_fields[full_key] = value;
    } else if (bool_fields.count(full_key)) {
      *bool_fields[full_key] =
          (value == "true" || value == "1" || value == "on" || value == "all");
    }
  }

  return config;
}

} // namespace parser
