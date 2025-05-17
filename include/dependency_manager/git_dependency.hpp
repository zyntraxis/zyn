#pragma once

#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace dependency_manager {
std::string exec(const std::string &cmd);
std::string get_commit_rev(const std::string &repo_path);
std::string get_latest_commit_hash(const std::string &repo_path);
std::string get_commit_hash_from_tag(const std::string &tag,
                                     const std::string &repo_path);
std::string hash_directory(const fs::path &dir);
void write_lock(const std::string &name, const std::string &rev,
                const std::string &hash);
bool check_lock(const std::string &name, const std::string &rev,
                const std::string &hash);
void ensure_git_dep(const std::string &name, const std::string &git_url,
                    const std::string &tag = "");
void install_from_url(const std::string &url);
void install_all_from_config();
void find_include_dirs(const fs::path &basePath,
                       std::vector<std::string> &includes);
void update_git_dependency(const std::string &name, const std::string &url,
                           const std::string &tag);
void update_all_dependencies();
} // namespace dependency_manager