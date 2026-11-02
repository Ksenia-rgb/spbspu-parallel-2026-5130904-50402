#include "arguments.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>

namespace
{
  long long parseInteger(const char* text)
  {
    const int base = 10;
    char* end = nullptr;
    errno = 0;
    const long long value = std::strtoll(text, &end, base);
    if ((end == text) || (*end != '\0') || (errno == ERANGE))
    {
      throw std::invalid_argument("arguments must be integers");
    }
    return value;
  }
}

pozdnyakov::arguments_t pozdnyakov::parseArguments(int argc, const char* const* argv)
{
  const int threads_index = 1;
  const int tries_index = 2;
  const int seed_index = 3;
  if ((argc <= tries_index) || (argc > (seed_index + 1)))
  {
    throw std::invalid_argument("usage: lab threads tries [seed]");
  }
  const long long threads = parseInteger(argv[threads_index]);
  const long long tries = parseInteger(argv[tries_index]);
  const long long seed = (argc > seed_index) ? parseInteger(argv[seed_index]) : 0;
  if (threads < 0)
  {
    throw std::invalid_argument("threads must not be negative");
  }
  if (tries <= 0)
  {
    throw std::invalid_argument("tries must be positive");
  }
  if (seed < 0)
  {
    throw std::invalid_argument("seed must not be negative");
  }
  return {static_cast< std::size_t >(threads), static_cast< std::size_t >(tries), static_cast< std::size_t >(seed)};
}
