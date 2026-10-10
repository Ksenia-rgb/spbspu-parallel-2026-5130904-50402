#ifndef AREA_HPP
#define AREA_HPP
#include <cstddef>
#include <utility>
#include "shapes.hpp"
namespace dirko {
  std::pair< size_t, size_t > calculate(const circles_t &circles, point_t min, point_t max, size_t tests, size_t seed);
  std::pair< point_t, point_t > getBorders(const circles_t &circles);
  std::pair< double, double > area(const circles_t &circles, size_t threads, size_t tests, size_t seed);
}
#endif
