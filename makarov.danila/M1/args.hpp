#ifndef ARGS_HPP
#define ARGS_HPP

#include <cstddef>

namespace makarov
{
  struct Parameters
  {
    std::size_t threads;
    std::size_t tries;
    std::size_t seed;
  };

  Parameters parseParameters(int argc, const char* const* argv);
}

#endif
