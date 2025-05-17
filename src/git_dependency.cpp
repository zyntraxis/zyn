#include "../include/dependency_manager/git_dependency.hpp"
#include "../include/project_management/parser.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <openssl/sha.h>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
static std::mutex cout_mutex;

namespace dependency_manager {
std::string exec(const std::string &cmd) {
  std::vector<char> buffer(256);
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe)
    throw std::runtime_error("Failed to run command: " + cmd);
  while (fgets(buffer.data(), buffer.size(), pipe.get()))
    result += buffer.data();
  result.erase(result.find_last_not_of(" \n\r\t") + 1);
  return result;
}

std::string get_commit_hash(const fs::path &repo, const std::string &tag = {}) {
  if (!tag.empty()) {
    exec("git -C \"" + repo.string() + "\" fetch --tags");
    std::string hash =
        exec("git -C \"" + repo.string() + "\" rev-list -n 1 refs/tags/" + tag);
    if (hash.empty()) {
      throw std::runtime_error("Tag '" + tag +
                               "' not found in repo: " + repo.string());
    }
    return hash;
  }
  return exec("git -C \"" + repo.string() +
              "\" ls-remote origin HEAD | cut -f1");
}

std::string hex_string(const unsigned char *data, size_t len) {
  static const char hex_digits[] = "0123456789abcdef";
  std::string result;
  result.reserve(len * 2);
  for (size_t i = 0; i < len; ++i) {
    result.push_back(hex_digits[(data[i] >> 4) & 0xF]);
    result.push_back(hex_digits[data[i] & 0xF]);
  }
  return result;
}

std::string hash_directory(const fs::path &dir) {
  std::vector<fs::path> files;

  for (auto &p : fs::recursive_directory_iterator(dir)) {
    if (fs::is_regular_file(p)) {
      std::string ext = p.path().extension().string();
      if (ext == ".cpp" || ext == ".h" ||
          p.path().filename() == "CMakeLists.txt") {
        files.push_back(p.path());
      }
    }
  }

  std::sort(files.begin(), files.end());

  SHA256_CTX ctx;
  SHA256_Init(&ctx);

  for (const auto &file : files) {
    std::ifstream in(file, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
    SHA256_Update(&ctx, reinterpret_cast<const unsigned char *>(buffer.data()),
                  buffer.size());
  }

  unsigned char hash[32];
  SHA256_Final(hash, &ctx);

  return hex_string(hash, 32);
}

void write_lock(const fs::path &path, const std::string &rev,
                const std::string &hash) {
  fs::create_directories(path.parent_path());
  std::ofstream out(path);
  out << "rev=" << rev << '\n' << "sha256=" << hash << '\n';
}

bool check_lock(const std::string &name, const std::string &rev,
                const std::string &hash) {
  fs::path path = ".zyn/lock/" + name + ".lock";
  if (!fs::exists(path))
    return false;

  std::ifstream in(path);
  std::string line, file_rev, file_hash;
  while (std::getline(in, line)) {
    if (line.size() > 4 && line.compare(0, 4, "rev=") == 0)
      file_rev = line.substr(4);
    else if (line.size() > 7 && line.compare(0, 7, "sha256=") == 0)
      file_hash = line.substr(7);
  }

  return file_rev == rev && file_hash == hash;
}

void clone_if_missing(const fs::path &path, const std::string &url) {
  if (!fs::exists(path / ".git")) {
    std::string cmd = "git clone \"" + url + "\" \"" + path.string() + "\"";
    if (std::system(cmd.c_str()) != 0)
      throw std::runtime_error("Git clone failed: " + url);
  }
}

void checkout_commit(const fs::path &repo, const std::string &commit) {
  std::string cmd = "git -C \"" + repo.string() + "\" fetch origin " + commit +
                    " && git -C \"" + repo.string() + "\" reset --hard " +
                    commit;
  if (std::system(cmd.c_str()) != 0)
    throw std::runtime_error("Git checkout failed");
}

void build_cmake(const fs::path &source, const fs::path &build) {
  fs::create_directories(build);
  std::string cmake = "cmake -S \"" + source.string() + "\" -B \"" +
                      build.string() +
                      "\" -DCMAKE_POSITION_INDEPENDENT_CODE=ON";
  std::string make = "cmake --build \"" + build.string() + "\"";
  if (std::system(cmake.c_str()) != 0 || std::system(make.c_str()) != 0)
    throw std::runtime_error("Build failed");
}

bool check_lock_strict(const std::string &name, const std::string &expected_rev,
                       const std::string &expected_hash) {
  fs::path path = ".zyn/lock/" + name + ".lock";
  if (!fs::exists(path)) {
    std::cerr << "[Zyn] Missing lock file for: " << name << "\n";
    return false;
  }

  std::ifstream in(path);
  if (!in) {
    std::cerr << "[Zyn] Cannot open lock file: " << path << "\n";
    return false;
  }

  std::string line, rev, hash;
  int valid_lines = 0;

  while (std::getline(in, line)) {
    if (line.rfind("rev=", 0) == 0) {
      rev = line.substr(4);
      valid_lines++;
    } else if (line.rfind("sha256=", 0) == 0) {
      hash = line.substr(7);
      valid_lines++;
    } else {
      std::cerr << "[Zyn] Invalid line in lock file: " << line << "\n";
      return false;
    }
  }

  if (valid_lines != 2) {
    std::cerr << "[Zyn] Lock file must have exactly two valid lines.\n";
    return false;
  }

  if (rev.empty() || hash.empty()) {
    std::cerr << "[Zyn] Lock file is missing required fields.\n";
    return false;
  }

  if (rev != expected_rev) {
    std::cerr << "[Zyn] Lock file rev mismatch:\n  expected: " << expected_rev
              << "\n  found:    " << rev << "\n";
    return false;
  }

  if (hash != expected_hash) {
    std::cerr << "[Zyn] Lock file hash mismatch:\n  expected: " << expected_hash
              << "\n  found:    " << hash << "\n";
    return false;
  }

  return true;
}

void ensure_git_dep(const std::string &name, const std::string &url,
                    const std::string &tag) {
  try {
    fs::path base = ".zyn";
    fs::path dep_dir = base / "deps" / name;
    fs::path build_dir = base / "build" / name;
    fs::path lock_path = base / "lock" / (name + ".lock");

    clone_if_missing(dep_dir, url);
    std::string commit = get_commit_hash(dep_dir, tag);

    if (fs::exists(lock_path)) {
      checkout_commit(dep_dir, commit);
      std::string current_hash = hash_directory(dep_dir);

      if (check_lock_strict(name, commit, current_hash)) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[Zyn] " << name << " is up-to-date and locked.\n";
        return;
      } else {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "[Zyn] Lock mismatch for " << name << ".\n";
        std::cerr << "[Zyn] Aborting install. Use `zyn update` to refresh.\n";
        std::exit(1);
      }
    }

    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Zyn] Installing " << name << "...\n";
    }

    checkout_commit(dep_dir, commit);
    std::string new_hash = hash_directory(dep_dir);
    write_lock(lock_path, commit, new_hash);
    build_cmake(dep_dir, build_dir);

    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Zyn] " << name << " ready.\n";
    }
  } catch (const std::exception &ex) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cerr << "[Zyn] Error installing " << name << ": " << ex.what() << "\n";
  }
}

void find_include_dirs(const fs::path &basePath,
                       std::vector<std::string> &includes) {
  if (!fs::exists(basePath) || !fs::is_directory(basePath))
    return;

  for (const auto &entry : fs::recursive_directory_iterator(basePath)) {
    if (entry.is_directory()) {
      auto dirName = entry.path().filename().string();
      if (dirName == "include" || dirName == "Include") {
        includes.push_back(entry.path().string());
      }
    }
  }
}

void install_from_url(const std::string &input) {
  std::string url = input, tag;
  if (auto pos = input.find('@'); pos != std::string::npos) {
    url = input.substr(0, pos);
    tag = input.substr(pos + 1);
  }

  std::smatch match;
  std::regex re("github\\.com/[^/]+/([^/@]+)");
  std::string name;

  if (std::regex_search(url, match, re)) {
    name = match[1].str();
    if (name.size() >= 4 && name.compare(name.size() - 4, 4, ".git") == 0)
      name = name.substr(0, name.size() - 4);
  } else {
    name = "library";
  }

  project_management::Config cfg = project_management::parse("zyn.toml");

  if (cfg.dependencies.find(name) != cfg.dependencies.end()) {
    std::cout << "[Zyn] Dependency \"" << name
              << "\" already exists. Skipping add.\n";
    return;
  } else {
    project_management::Dependency dep{.git = url, .tag = tag};

    if (tag.empty()) {
      std::string tmp = ".zyn/tmp_" + name;
      std::string cmd = "git clone --bare \"" + url + "\" \"" + tmp + "\"";
      if (std::system(cmd.c_str()) != 0)
        throw std::runtime_error("Clone failed");
      tag = exec("git --git-dir=\"" + tmp + "\" describe --tags --abbrev=0");
      fs::remove_all(tmp);
      dep.tag = tag;
      std::cout << "[Zyn] Latest tag resolved: " << tag << "\n";
    } else {
      std::string test_tag = tag;
      if (tag.empty() == false && tag[0] != 'v') {
        test_tag = "v" + tag;
      }

      std::string tmp = ".zyn/tmp_" + name;
      std::string clone_cmd =
          "git clone --bare \"" + url + "\" \"" + tmp + "\"";
      if (std::system(clone_cmd.c_str()) != 0)
        throw std::runtime_error("Clone failed");

      std::string check_tag_cmd = "git --git-dir=\"" + tmp +
                                  "\" rev-parse --verify --quiet refs/tags/" +
                                  test_tag;
      std::string verify_result = exec(check_tag_cmd);

      fs::remove_all(tmp);

      if (!verify_result.empty()) {
        tag = test_tag;
      }
      dep.tag = tag;
    }

    cfg.dependencies[name] = dep;
    project_management::save("zyn.toml", cfg);
  }

  ensure_git_dep(name, url, tag);
  std::vector<std::string> include_dirs;

  find_include_dirs(".zyn/deps", include_dirs);
  find_include_dirs(".zyn/build", include_dirs);

  for (const auto &dir : include_dirs) {
    std::cout << dir << "\n";
  }
}

void install_all_from_config() {
  project_management::Config cfg = project_management::parse("zyn.toml");

  std::vector<std::future<void>> futures;

  for (const auto &[name, dep] : cfg.dependencies) {
    if (!dep.git.empty()) {
      futures.push_back(std::async(std::launch::async, ensure_git_dep, name,
                                   dep.git, dep.tag));
    } else if (!dep.path.empty()) {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Zyn] Skipping local/path dependency \"" << name << "\"\n";
    } else {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "[Zyn] Dependency \"" << name
                << "\" has no git or path. Skipping.\n";
    }
  }

  for (auto &fut : futures) {
    fut.get();
  }
}

void update_git_dependency(const std::string &name, const std::string &url,
                           const std::string &tag) {
  fs::path dep_dir = ".zyn/deps/" + name;
  fs::path build_dir = ".zyn/build/" + name;
  fs::path lock_path = ".zyn/lock/" + name + ".lock";

  clone_if_missing(dep_dir, url);
  std::string latest_commit = get_commit_hash(dep_dir, tag);
  checkout_commit(dep_dir, latest_commit);
  std::string new_hash = hash_directory(dep_dir);

  bool needs_update = !check_lock(name, latest_commit, new_hash);
  if (needs_update) {
    std::cout << "[Zyn] Updating " << name << "...\n";
    write_lock(lock_path, latest_commit, new_hash);
    build_cmake(dep_dir, build_dir);
    std::cout << "[Zyn] " << name << " updated.\n";
  } else {
    std::cout << "[Zyn] " << name << " is already up-to-date.\n";
  }
}

void update_all_dependencies() {
  auto config = project_management::parse("zyn.toml");

  for (const auto &[name, dep] : config.dependencies) {
    std::string url = dep.git;
    std::string tag = dep.tag;

    if (url.empty())
      continue;

    update_git_dependency(name, url, tag);
  }
}
} // namespace dependency_manager