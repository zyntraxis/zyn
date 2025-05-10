#include "../include/builder.hpp"
#include <algorithm>
#include <iostream>

std::string extract_name(const std::string &url) {
  std::string name = url;

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

bool clone_dependency(const std::string &url, const fs::path &target_dir) {
  if (fs::exists(target_dir)) {
    std::cout << "[git] Dependency already exists: " << target_dir << "\n";
    return true;
  }

  std::cout << "[git] Cloning " << url << " into " << target_dir << "\n";
  git_libgit2_init();

  git_repository *repo = nullptr;
  int error = git_clone(&repo, url.c_str(), target_dir.c_str(), nullptr);
  git_libgit2_shutdown();

  if (error != 0) {
    std::cerr << "[error] Failed to clone " << url << "\n";
    return false;
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

  std::cout << "[info] No build system detected, skipping build\n";
  return false;
}

void process_dependencies(const std::vector<std::string> &deps) {
  fs::path deps_dir = "dependencies";

  for (const auto &dep : deps) {
    std::string name = extract_name(dep);
    fs::path lib_path = deps_dir / name;

    if (!fs::exists(lib_path)) {
      if (!clone_dependency(dep, lib_path)) {
        continue;
      }
    }

    if (!detect_and_build(lib_path)) {
      std::cerr << "[error] Failed to build " << name << "\n";
    }
  }
}

std::vector<fs::path> find_include_dirs(const fs::path &dependencies_path) {
  std::vector<fs::path> include_paths;

  if (!fs::exists(dependencies_path) || !fs::is_directory(dependencies_path)) {
    std::cerr << "Invalid dependencies path: " << dependencies_path << '\n';
    return include_paths;
  }

  for (const auto &lib_dir : fs::directory_iterator(dependencies_path)) {
    if (!fs::is_directory(lib_dir))
      continue;

    for (const auto &entry : fs::recursive_directory_iterator(lib_dir)) {
      if (entry.is_directory() && entry.path().filename() == "include") {
        include_paths.push_back(entry.path());
        break;
      }
    }
  }

  return include_paths;
}
