#pragma once
#include <string>
#include <vector>

struct Config {
  // [config]
  std::string version;
  std::string project;
  std::string language;
  std::string standard;
  std::string build_type;
  std::string compiler;
  std::string warnings;
  std::string optimization;
  std::string c_cache;

  // [directories]
  std::string sources_dir;
  std::string include_dir;
  std::string build_dir;

  // [dependencies]
  std::vector<std::string> dependencies;

  // [linting]
  std::string linter;
  std::string linter_config_file;
  std::string linter_enable_checks;
  std::string linter_treat_warnings_as_errors;

  // [analysis]
  std::string static_analyzer;
  std::string cppcheck_enable;
  std::string cppcheck_inconclusive;
  std::string cppcheck_force;

  // [profiling]
  std::string profiling_tool;
};
