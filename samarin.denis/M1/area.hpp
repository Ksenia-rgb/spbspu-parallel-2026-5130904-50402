#ifndef AREA_HPP
#define AREA_HPP

#include <cstddef>
#include <utility>
#include "shape.hpp"

namespace samarin {
  using areas_t = std::pair< double, double >;

  areas_t getAreas(const shapes_t & shapes, std::size_t threads, std::size_t tries, std::size_t seed);
}

#endif
