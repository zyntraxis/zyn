#include <string>
#include <unordered_map>
#include <vector>

namespace project_management {
struct Dependency {
  std::string git;
  std::string tag;
  std::string path;
};

struct Config {
  std::string version;
  std::string name;
  std::string language;
  std::string standard;
  std::string compiler;
  std::string sources;
  std::string include;
  std::string build;
  std::unordered_map<std::string, Dependency> dependencies;
  std::vector<std::string> libraries;
  std::vector<std::string> lib_dirs;
};
Config parse(std::string config_file);
void save(const std::string &path, const Config &config);
} // namespace project_management