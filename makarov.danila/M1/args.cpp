#include "args.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace {
  const int threadsIndex = 1;
  const int triesIndex = 2;
  const int seedIndex = 3;
  const int argsWithoutSeed = seedIndex;
  const int argsWithSeed = seedIndex + 1;

  long long parseInteger(const char* str, const std::string& name)
  {
    const std::string text(str);
    std::size_t parsed = 0;
    long long value = 0;
    try {
      value = std::stoll(text, std::addressof(parsed));
    } catch (const std::logic_error&) {
      throw std::invalid_argument(name + " is not a valid integer");
    }
    if (parsed != text.size()) {
      throw std::invalid_argument(name + " is not a valid integer");
    }
    return value;
  }
}

makarov::Parameters makarov::parseParameters(int argc, const char* const* argv)
{
  if (argc != argsWithoutSeed && argc != argsWithSeed) {
    throw std::invalid_argument("wrong number of arguments, expected: threads tries [seed]");
  }
  const long long threads = parseInteger(argv[threadsIndex], "threads");
  if (threads <= 0) {
    throw std::invalid_argument("threads count is not positive");
  }
  const long long tries = parseInteger(argv[triesIndex], "tries");
  if (tries <= 0) {
    throw std::invalid_argument("tries count is not positive");
  }
  long long seed = 0;
  if (argc == argsWithSeed) {
    seed = parseInteger(argv[seedIndex], "seed");
  }
  if (seed < 0) {
    throw std::invalid_argument("seed is negative");
  }
  return {static_cast< std::size_t >(threads), static_cast< std::size_t >(tries), static_cast< std::size_t >(seed)};
}
