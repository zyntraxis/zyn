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

  std::unordered_map<std::string, std::string *> field_map;

  field_map.insert({"config.version", &config.version});
  field_map.insert({"config.project", &config.project});
  field_map.insert({"config.language", &config.language});
  field_map.insert({"config.standard", &config.standard});
  field_map.insert({"config.build_type", &config.build_type});
  field_map.insert({"config.compiler", &config.compiler});
  field_map.insert({"config.warnings", &config.warnings});
  field_map.insert({"config.optimization", &config.optimization});
  field_map.insert({"config.c_cache", &config.c_cache});

  field_map.insert({"directories.sources", &config.sources_dir});
  field_map.insert({"directories.include", &config.include_dir});
  field_map.insert({"directories.build", &config.build_dir});

  field_map.insert({"linting.linter", &config.linter});
  field_map.insert({"linting.config_file", &config.linter_config_file});
  field_map.insert({"linting.enable_checks", &config.linter_enable_checks});
  field_map.insert({"linting.treat_warnings_as_errors",
                    &config.linter_treat_warnings_as_errors});

  field_map.insert({"analysis.static_analyzer", &config.static_analyzer});
  field_map.insert({"analysis.cppcheck_enable", &config.cppcheck_enable});
  field_map.insert(
      {"analysis.cppcheck_inconclusive", &config.cppcheck_inconclusive});
  field_map.insert({"analysis.cppcheck_force", &config.cppcheck_force});

  field_map.insert({"profiling.tool", &config.profiling_tool});

  while (std::getline(file, line)) {
    trim(line);
    if (line.empty() || line[0] == '#')
      continue;

    if (line.front() == '[' && line.back() == ']') {
      section = line.substr(1, line.size() - 2);
      trim(section);
    } else {
      auto eq_pos = line.find('=');

      if (section == "dependencies") {
        config.dependencies.push_back(line);
        continue;
      }

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