#include "../include/parser.hpp"
#include <fstream>
#include <future>
#include <mutex>
#include <unordered_map>

namespace parser {

namespace {
void trim(std::string &s) {
  s.erase(0, s.find_first_not_of(" \t\r\n"));
  s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

bool parse_bool(const std::string &value) {
  return (value == "true" || value == "1" || value == "on" || value == "all");
}

std::vector<std::string> read_lines_parallel(const std::string &filename) {
  std::ifstream file(filename);
  std::vector<std::string> lines;
  std::string line;

  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  return lines;
}
} // namespace

Config parse_config(const std::string &filename) {
  Config config;
  std::mutex config_mutex;
  std::string current_section;
  std::mutex section_mutex;

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

  auto lines = read_lines_parallel(filename);
  std::vector<std::future<void>> futures;

  for (auto &line : lines) {
    futures.emplace_back(std::async(
        std::launch::async, [&line, &config, &config_mutex, &str_fields,
                             &bool_fields, &current_section, &section_mutex] {
          trim(line);
          if (line.empty() || line[0] == '#')
            return;

          std::string local_section;
          {
            std::lock_guard<std::mutex> lock(section_mutex);
            local_section = current_section;
          }

          if (line.front() == '[' && line.back() == ']') {
            std::string section = line.substr(1, line.size() - 2);
            trim(section);
            {
              std::lock_guard<std::mutex> lock(section_mutex);
              current_section = section;
            }
            return;
          }

          if (local_section == "dependencies") {
            std::lock_guard<std::mutex> lock(config_mutex);
            config.dependencies.push_back(line);
            return;
          }

          auto eq_pos = line.find('=');
          if (eq_pos == std::string::npos)
            return;

          std::string key = line.substr(0, eq_pos);
          std::string value = line.substr(eq_pos + 1);
          trim(key);
          trim(value);

          std::string full_key = local_section + "." + key;

          std::lock_guard<std::mutex> lock(config_mutex);

          if (str_fields.count(full_key)) {
            *str_fields[full_key] = value;
          } else if (bool_fields.count(full_key)) {
            *bool_fields[full_key] = parse_bool(value);
          }
        }));
  }

  for (auto &f : futures) {
    f.get();
  }

  return config;
}

} // namespace parser
