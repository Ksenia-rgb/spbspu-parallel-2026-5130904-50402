#include <cstdlib>
#include <iostream>

namespace karpenkov {
constexpr size_t count_of_args = 4;
constexpr size_t invalid_args_count_error = 1;
constexpr size_t base_of_ten = 10;
constexpr size_t invalid_args = 2;
size_t stringToSizeT(const char *lms) {
  char *end = nullptr;
  const unsigned long long rms = std::strtoull(lms, &end, base_of_ten);
  return static_cast<size_t>(rms);
}
}; // namespace karpenkov

int main(int argc, const char *argv[]) {
  if (argc < karpenkov::count_of_args) {
    std::cerr << "Invalid count of arguments";
    return karpenkov::invalid_args_count_error;
  }
  size_t threads = 0, tries = 0, seed = 0;
  try {
    threads = karpenkov::stringToSizeT(argv[1]);
    tries = karpenkov::stringToSizeT(argv[2]);
    seed = karpenkov::stringToSizeT(argv[3]);
  } catch (...) {
    std::cerr << "Invalid argument";
    return karpenkov::invalid_args;
  }
}
