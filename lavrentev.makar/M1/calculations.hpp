#ifndef CALCULATIONS_HPP
#define CALCULATIONS_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include "circle.hpp"
#include "polygon.hpp"

namespace lavrentev
{
  using count_pair_t = std::pair< std::size_t, std::size_t >;
  using area_pair_t = std::pair< double, double >;
  using f_t = std::vector< Circle >;

  f_t readInput(Polygon &pg);
  count_pair_t calculate(const f_t &figures, const Polygon &pg, std::size_t tries, int seed);
  std::size_t countInside(const f_t &figures, double x, double y);
  area_pair_t area(const f_t &figures, const Polygon &pg, std::size_t threads, std::size_t tries, int seed);
}

#endif
