#ifndef AREA_HPP
#define AREA_HPP
#include <cstddef>
#include <utility>
#include "ellipse.hpp"

namespace shirokov
{
  std::pair< size_t, size_t > calc(const ellipse_list& ellipses, size_t tries, size_t seed, const MinMaxes& min_maxes);
  const std::pair< double, double > area(const ellipse_list& ellipses, size_t threads, size_t tries, size_t seed);
}

#endif
