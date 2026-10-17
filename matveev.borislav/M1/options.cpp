#include "options.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
  static std::uint64_t parseNonNegative(const char* const text)
  {
    const std::string value(text);
    const bool has_sign = !value.empty() && ((value.front() == '+') || (value.front() == '-'));
    const std::size_t start = has_sign ? 1 : 0;
    if ((start == value.size()) || (value.find_first_not_of("0123456789", start) != std::string::npos))
    {
      throw std::invalid_argument("Expected a non-negative integer argument");
    }
    const unsigned long long number = std::stoull(value.substr(start));
    if ((value.front() == '-') && (number != 0))
    {
      throw std::invalid_argument("Expected a non-negative integer argument");
    }
    if (number > std::numeric_limits< std::uint64_t >::max())
    {
      throw std::out_of_range("Argument is too large");
    }
    return static_cast< std::uint64_t >(number);
  }
}

matveev::options_t matveev::parseArguments(const int argc, const char* const* const argv)
{
  constexpr int required_arguments = 3;
  constexpr int optional_arguments = 4;
  if ((argc != required_arguments) && (argc != optional_arguments))
  {
    throw std::invalid_argument("Usage: lab threads tries [seed]");
  }

  const std::uint64_t threads = parseNonNegative(argv[1]);
  const std::uint64_t tries = parseNonNegative(argv[2]);
  const std::uint64_t seed = (argc == optional_arguments) ? parseNonNegative(argv[3]) : 0;
  if (tries == 0)
  {
    throw std::invalid_argument("The number of trials must be positive");
  }
  if (threads > std::numeric_limits< std::size_t >::max())
  {
    throw std::out_of_range("The number of threads is too large");
  }
  return {static_cast< std::size_t >(threads), tries, seed};
}

