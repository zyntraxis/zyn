#include "../include/project_management/assembly_cache.hpp"
#include <filesystem>
#include <fstream>
#include <openssl/sha.h>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace project_management {

std::string hash_file_contents(const fs::path &file_path) {
  std::ifstream file(file_path, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Could not open file: " + file_path.string());
  }

  SHA256_CTX sha_context;
  SHA256_Init(&sha_context);

  char buffer[4096];
  while (file.read(buffer, sizeof(buffer))) {
    SHA256_Update(&sha_context, buffer, file.gcount());
  }
  SHA256_Update(&sha_context, buffer, file.gcount());

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash, &sha_context);

  std::stringstream ss;
  for (unsigned char byte : hash) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }

  return ss.str();
}

std::string hash_source_files(const Config &config) {
  SHA256_CTX sha_context;
  SHA256_Init(&sha_context);

  // Hash all source files
  for (const auto &entry : fs::recursive_directory_iterator(config.sources)) {
    if (entry.is_regular_file() &&
        entry.path().extension() == "." + config.language) {
      std::string file_hash = hash_file_contents(entry.path());
      SHA256_Update(&sha_context, file_hash.c_str(), file_hash.size());
    }
  }

  // Hash all header files
  for (const auto &entry : fs::recursive_directory_iterator(config.include)) {
    if (entry.is_regular_file() && entry.path().extension() == ".h") {
      std::string file_hash = hash_file_contents(entry.path());
      SHA256_Update(&sha_context, file_hash.c_str(), file_hash.size());
    }
  }

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash, &sha_context);

  std::stringstream ss;
  for (unsigned char byte : hash) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }

  return ss.str();
}

bool needs_rebuild(const Config &config) {
  fs::path cache_dir = ".zyn/cache";
  fs::path hash_file = cache_dir / "source_hashes.txt";
  fs::path executable = ".zyn/build/" + config.name;

  // If no executable exists, we definitely need to rebuild
  if (!fs::exists(executable)) {
    return true;
  }

  // If no hash file exists, we need to rebuild
  if (!fs::exists(hash_file)) {
    return true;
  }

  // Read the stored hash
  std::ifstream in(hash_file);
  std::string stored_hash;
  std::getline(in, stored_hash);

  // Calculate current hash
  std::string current_hash = hash_source_files(config);

  // If hashes differ, we need to rebuild
  if (stored_hash != current_hash) {
    return true;
  }

  // Check if any dependency has been updated
  auto executable_time = fs::last_write_time(executable);
  for (const auto &[name, dep] : config.dependencies) {
    if (!dep.path.empty()) {
      fs::path dep_path = dep.path;
      if (fs::is_directory(dep_path)) {
        for (const auto &entry : fs::recursive_directory_iterator(dep_path)) {
          if (entry.is_regular_file() &&
              (entry.path().extension() == ".h" ||
               entry.path().extension() == ".cpp" ||
               entry.path().filename() == "CMakeLists.txt")) {
            if (fs::last_write_time(entry) > executable_time) {
              return true;
            }
          }
        }
      }
    }
  }

  return false;
}

void update_cache(const Config &config) {
  fs::path cache_dir = ".zyn/cache";
  fs::create_directories(cache_dir);

  std::string current_hash = hash_source_files(config);
  std::ofstream out(cache_dir / "source_hashes.txt");
  out << current_hash;
}

} // namespace project_management