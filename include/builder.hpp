#pragma once

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <git2.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::string extract_name(const std::string &url);
bool clone_dependency(const std::string &url, const fs::path &target_dir);
bool build_with_cmake(const fs::path &repo_path);
bool build_with_make(const fs::path &repo_path);
bool build_with_autotools(const fs::path &repo_path);
bool build_with_ninja(const fs::path &repo_path);
bool detect_and_build(const fs::path &repo_path);
void process_dependencies(const std::vector<std::string> &deps);
std::vector<fs::path> find_include_dirs(const fs::path &dependencies_path);
bool is_dependency_exists(const std::string &git_url,
                          const std::string &config_file);
void add_dependencies(const std::vector<std::string> &git_urls,
                      const std::string &config_file);
void clean_dependencies(const fs::path &deps_dir);
void clean_build_directory(const fs::path &build_dir);
void clean_project(const std::string &build_dir,
                   const std::string &dependencies_dir);