#include "../include/utils/utils.hpp"
#include <iostream>

namespace utils {

std::string input_with_prompt(const std::string &prompt) {
  std::cout << prompt;
  std::string input;
  std::getline(std::cin, input);
  return input;
}

} // namespace utils