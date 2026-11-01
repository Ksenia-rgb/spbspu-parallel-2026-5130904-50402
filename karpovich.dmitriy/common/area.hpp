#ifndef AREA_HPP
#define AREA_HPP

#include <cstddef>
#include <utility>
#include <vector>
#include "shape.hpp"

namespace karpovich
{
  using shapes_t = std::vector< shape_t >;

  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed = 0);
}

#endif
