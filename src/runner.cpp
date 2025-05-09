#include "../include/runner.hpp"
#include "../include/config.hpp"
#include "../include/parser.hpp"
#include <sstream>

namespace runner {
void run(const Config &parsed_config) {
  Config config = parser::parse_config("config.zyn");

  std::stringstream cmd;
  std::string compile_language = config.language == "c" ? "c" : "c++";

  cmd << config.compiler << " -std=" << compile_language << config.standard
      << " " << config.sources_dir << "/*." << config.language;

  if (config.build_type == "release") {
    cmd << " -O3 -march=native -flto -funroll-loops -ftree-vectorize "
           "-D_FORTIFY_SOURCE=2 -fstack-protector-all ";
  } else {
    cmd << " -g ";
  }

  cmd << " -o " << config.project;
  std::system(cmd.str().c_str());
  std::system(("./" + config.project).c_str());
}
} // namespace runner