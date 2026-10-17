#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstddef>
#include <cstdint>

namespace matveev
{
  struct options_t
  {
    std::size_t threads;
    std::uint64_t tries;
    std::uint64_t seed;
  };

  options_t parseArguments(int argc, const char* const* argv);
}

#endif
