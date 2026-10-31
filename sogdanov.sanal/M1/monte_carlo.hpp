#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <cstddef>
#include <vector>

#include "figure.hpp"

namespace sogdanov
{
  struct Areas
  {
    long double coverage;
    long double intersection;
  };

  Areas estimateAreas(
      const std::vector< Figure >& figures, std::size_t thread_count, std::size_t tries, std::size_t seed);
}

#endif
