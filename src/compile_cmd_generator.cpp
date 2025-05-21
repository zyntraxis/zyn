#include "../include/project_management/compile_cmd_generator.hpp"
#include "../include/dependency_manager/git_dependency.hpp"
#include "../include/project_management/parser.hpp"
#include <filesystem>
#include <vector>
#include <sstream>

#ifdef _WIN32
#define EXE_SUFFIX ".exe"
#else
#define EXE_SUFFIX ""
#endif

namespace fs = std::filesystem;

namespace project_management
{

  std::stringstream generate_compile_cmd()
  {
    const project_management::Config cfg = project_management::parse("zyn.toml");
    std::stringstream cmd;

    cmd << cfg.compiler << " -std=" << cfg.standard;

    for (auto &p : fs::recursive_directory_iterator(cfg.sources))
    {
      if (p.is_regular_file() && p.path().extension() == ("." + cfg.language))
      {
        cmd << " " << p.path().string();
      }
    }

    std::string output_path = ".zyn/build/" + cfg.name + EXE_SUFFIX;
    cmd << " -o " << output_path;

    cmd << " -I" << cfg.include;

    std::vector<std::string> include_dirs;
    include_dirs.reserve(cfg.dependencies.size() + 2);

    for (const auto &[_, dep] : cfg.dependencies)
    {
      if (!dep.path.empty())
      {
        dependency_manager::find_include_dirs(dep.path, include_dirs);
      }
    }

    dependency_manager::find_include_dirs(".zyn/deps", include_dirs);
    dependency_manager::find_include_dirs(".zyn/build", include_dirs);

    for (const auto &dir : include_dirs)
    {
      cmd << " -I" << dir;
    }

    for (const auto &lib_dir : cfg.lib_dirs)
    {
      cmd << " -L" << lib_dir;
    }

    for (const auto &lib : cfg.libraries)
    {
      cmd << " -l" << lib;
    }

    return cmd;
  }

} // namespace project_management
