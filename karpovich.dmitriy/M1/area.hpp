#ifndef KARPOVICH_AREA_HPP
#define KARPOVICH_AREA_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include "shape.hpp"

namespace karpovich
{

  struct AreaResult
  {
    double coverArea;
    double intersectionArea;
  };
  AreaResult area(const std::vector< std::unique_ptr< Shape > > &shapes, size_t threads, size_t tests, size_t seed = 0);
}

#endif
