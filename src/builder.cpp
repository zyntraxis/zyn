#include "../include/builder.hpp"
#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

std::mutex io_mutex;

std::pair<std::string, std::string> split_url_and_ref(const std::string &url) {
  size_t at_pos = url.find('@');
  if (at_pos == std::string::npos) {
    return {url, ""};
  }
  return {url.substr(0, at_pos), url.substr(at_pos + 1)};
}

std::string extract_name(const std::string &url) {
  auto [clean_url, ref] = split_url_and_ref(url);
  std::string name = clean_url;

  if (name.size() > 4 && name.substr(name.size() - 4) == ".git") {
    name = name.substr(0, name.size() - 4);
  }

  size_t last_slash = name.find_last_of("/:");
  if (last_slash != std::string::npos) {
    name = name.substr(last_slash + 1);
  }

  name.erase(std::remove_if(name.begin(), name.end(),
                            [](char c) {
                              return !std::isalnum(c) && c != '_' && c != '-';
                            }),
             name.end());

  return name.empty() ? "unknown" : name;
}

bool clone_dependency(const std::string &full_url, const fs::path &target_dir) {
  if (fs::exists(target_dir)) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout << "\033[36m[git] Dependency already exists: " << target_dir
              << "\033[0m\n";
    return true;
  }

  auto [url, ref] = split_url_and_ref(full_url);
  {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout << "\033[36m[git] Cloning " << url << " into " << target_dir
              << "\033[0m\n";
  }

  std::string clone_cmd = "git clone " + url + " " + target_dir.string();
  if (std::system(clone_cmd.c_str()) != 0) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cerr << "\033[31m[error] Failed to clone " << url << "\n";
    return false;
  }

  if (!ref.empty()) {
    {
      std::lock_guard<std::mutex> lock(io_mutex);
      std::cout << "\033[36m[git] Checking out " << ref << "\033[0m\n";
    }
    std::string checkout_cmd =
        "cd " + target_dir.string() + " && git checkout " + ref;
    if (std::system(checkout_cmd.c_str()) != 0) {
      std::lock_guard<std::mutex> lock(io_mutex);
      std::cerr << "\033[31m[error] Failed to checkout " << ref << "\033[0m\n";
      return false;
    }
  }

  return true;
}

bool build_with_cmake(const fs::path &repo_path) {
  fs::path build_dir = repo_path / "build";
  fs::create_directory(build_dir);

  std::string cmake_cmd = "cmake -S " + repo_path.string() + " -B " +
                          build_dir.string() +
                          " -DCMAKE_INSTALL_PREFIX=" + build_dir.string();
  if (std::system(cmake_cmd.c_str()) != 0)
    return false;

  std::string build_cmd =
      "cmake --build " + build_dir.string() + " --target install";
  return std::system(build_cmd.c_str()) == 0;
}

bool build_with_make(const fs::path &repo_path) {
  std::string build_cmd = "cd " + repo_path.string() + " && make";
  return std::system(build_cmd.c_str()) == 0;
}

bool build_with_autotools(const fs::path &repo_path) {
  std::string cmd =
      "cd " + repo_path.string() + " && ./configure && make && make install";
  return std::system(cmd.c_str()) == 0;
}

bool build_with_ninja(const fs::path &repo_path) {
  fs::path build_dir = repo_path / "build";
  fs::create_directory(build_dir);

  std::string configure_cmd = "cmake -S " + repo_path.string() + " -B " +
                              build_dir.string() + " -G Ninja";
  if (std::system(configure_cmd.c_str()) != 0)
    return false;

  std::string build_cmd = "ninja -C " + build_dir.string();
  return std::system(build_cmd.c_str()) == 0;
}

bool detect_and_build(const fs::path &repo_path) {
  if (fs::exists(repo_path / "build.ninja")) {
    return build_with_ninja(repo_path);
  } else if (fs::exists(repo_path / "CMakeLists.txt")) {
    return build_with_cmake(repo_path);
  } else if (fs::exists(repo_path / "configure")) {
    return build_with_autotools(repo_path);
  } else if (fs::exists(repo_path / "Makefile") ||
             fs::exists(repo_path / "makefile")) {
    return build_with_make(repo_path);
  }

  {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout
        << "\033[32m[info] No build system detected, skipping build\033[0m\n";
  }
  return false;
}

void process_dependencies(const std::vector<std::string> &deps) {
  fs::path deps_dir = "dependencies";
  std::vector<std::future<void>> futures;

  for (const auto &dep : deps) {
    futures.push_back(std::async(std::launch::async, [&, dep] {
      std::string name = extract_name(dep);
      fs::path lib_path = deps_dir / name;

      if (!fs::exists(lib_path)) {
        if (!clone_dependency(dep, lib_path)) {
          return;
        }
      }

      if (!detect_and_build(lib_path)) {
        std::lock_guard<std::mutex> lock(io_mutex);
        std::cerr << "\033[31m[error] Failed to build " << name << "\033[0m\n";
      }
    }));
  }

  for (auto &fut : futures) {
    fut.get();
  }
}

std::vector<fs::path> find_include_dirs(const fs::path &dependencies_path) {
  std::vector<fs::path> include_paths;
  std::mutex include_mutex;

  if (!fs::exists(dependencies_path) || !fs::is_directory(dependencies_path)) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cerr << "\033[31mInvalid dependencies path: " << dependencies_path
              << "\033[0m\n";
    return include_paths;
  }

  std::vector<std::future<void>> futures;
  for (const auto &lib_dir : fs::directory_iterator(dependencies_path)) {
    if (!fs::is_directory(lib_dir))
      continue;

    futures.push_back(std::async(
        std::launch::async, [&include_paths, &include_mutex, lib_dir] {
          for (const auto &entry : fs::recursive_directory_iterator(lib_dir)) {
            if (entry.is_directory() && entry.path().filename() == "include") {
              std::lock_guard<std::mutex> lock(include_mutex);
              include_paths.push_back(entry.path());
              break;
            }
          }
        }));
  }

  for (auto &fut : futures)
    fut.get();
  return include_paths;
}

void add_dependencies(const std::vector<std::string> &deps,
                      const std::string &config_file) {
  std::ifstream file(config_file);
  if (!file.is_open()) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cerr << "\033[31m[error] Cannot open config file: " << config_file
              << "\033[0m\n";
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  std::string dependencies_marker = "[dependencies]";
  size_t dep_pos = content.find(dependencies_marker);

  if (dep_pos == std::string::npos) {
    content += "\n" + dependencies_marker + "\n";
    dep_pos = content.size();
  } else {
    dep_pos += dependencies_marker.size();
  }

  for (const std::string &dep : deps) {
    content.insert(dep_pos, "\n" + dep);
    dep_pos += dep.size() + 1;
  }

  std::ofstream out_file(config_file);
  if (!out_file.is_open()) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cerr << "\033[31m[error] Cannot open config file for writing: "
              << config_file << "\033[0m\n";
    return;
  }
  out_file << content;
  std::lock_guard<std::mutex> lock(io_mutex);
  std::cout << "\033[32m[info] Dependencies added successfully.\033[0m\n";
}

void clean_dependencies(const fs::path &deps_dir) {
  if (fs::exists(deps_dir)) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout << "\033[32m[clean] Removing dependencies directory: " << deps_dir
              << "\033[0m\n";
    fs::remove_all(deps_dir);
  }
}

void clean_build_directory(const fs::path &build_dir) {
  if (fs::exists(build_dir)) {
    std::lock_guard<std::mutex> lock(io_mutex);
    std::cout << "\033[32m[clean] Removing build directory: " << build_dir
              << "\033[0m\n";
    fs::remove_all(build_dir);
  }
}

void clean_project(const std::string &build_dir,
                   const std::string &dependencies_dir) {
  std::vector<std::future<void>> tasks;
  tasks.push_back(std::async(std::launch::async,
                             [&] { clean_build_directory(build_dir); }));
  tasks.push_back(std::async(std::launch::async,
                             [&] { clean_dependencies(dependencies_dir); }));

  for (auto &task : tasks)
    task.get();

  std::lock_guard<std::mutex> lock(io_mutex);
  std::cout << "\033[32m[clean] Project cleaned successfully\033[0m\n";
}