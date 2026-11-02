#ifndef CHILD_HPP
#define CHILD_HPP

#include <cstddef>
#include "pipe.hpp"

namespace novikov
{
  void childProcess(DuplexChannel channel, size_t seed);
}

#endif
