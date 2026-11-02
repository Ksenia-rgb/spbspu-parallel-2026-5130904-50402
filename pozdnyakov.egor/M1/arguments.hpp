#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <cstddef>

namespace pozdnyakov
{
  struct arguments_t
  {
    std::size_t threads;
    std::size_t tries;
    std::size_t seed;
  };

  arguments_t parseArguments(int argc, const char* const* argv);
}

#endif
