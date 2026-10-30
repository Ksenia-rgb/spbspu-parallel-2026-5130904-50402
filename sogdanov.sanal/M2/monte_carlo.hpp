#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <stddef.h>
#include <vector>

#include "figure.hpp"

namespace sogdanov
{
  struct Areas
  {
    long double coverage;
    long double intersection;
  };

  Bounds findGeneralBounds(const std::vector< Figure >& figures);

  Areas estimateAreas(const std::vector< Figure >& figures,
      size_t thread_count, size_t tries, size_t seed);
}

#endif
