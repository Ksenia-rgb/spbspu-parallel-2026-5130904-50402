#ifndef MATVEEV_MONTE_CARLO_HPP
#define MATVEEV_MONTE_CARLO_HPP

#include <cstdint>
#include <vector>
#include "shape.hpp"

namespace matveev
{
  std::uint64_t countHits(const std::vector< shape_t > &shapes, const frame_t &frame,
      std::uint64_t tries, std::uint64_t seed);
  double calculateArea(const std::vector< shape_t > &shapes, std::uint64_t threads,
      std::uint64_t tries, std::uint64_t seed);
}

#endif
