#ifndef AREA_HPP
#define AREA_HPP

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "shape.hpp"

namespace karpovich
{
  struct rectangle_t
  {
    point_t max;
    point_t min;
  };

  using shapes_t = std::vector< std::unique_ptr< Shape > >;
  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed = 0);
  rectangle_t getBoundingBox(const shapes_t &shapes);
}

#endif
