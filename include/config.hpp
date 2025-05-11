#pragma once
#include <string>
#include <vector>

struct LintingConfig {
  std::string linter;
  std::string config_file;
  bool enable_checks;
  bool treat_warnings_as_errors;
};

struct AnalysisConfig {
  std::string static_analyzer;
  bool cppcheck_enable;
  bool cppcheck_inconclusive;
  bool cppcheck_force;
};

struct ProfilingConfig {
  std::string tool;
};

struct Config {
  std::string version;
  std::string project;
  std::string language;
  std::string standard;
  std::string build_type;
  std::string compiler;
  std::string warnings;
  std::string optimization;
  std::string c_cache;
  std::string sources_dir;
  std::string include_dir;
  std::string build_dir;
  std::vector<std::string> dependencies;
  LintingConfig linting;
  AnalysisConfig analysis;
  ProfilingConfig profiling;
};
