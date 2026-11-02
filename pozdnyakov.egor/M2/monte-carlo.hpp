#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <cstddef>
#include "shapes.hpp"

namespace pozdnyakov
{
  double computeCoverage(const ellipses_t& ellipses, std::size_t threads, std::size_t tries, std::size_t seed);
}

#endif
