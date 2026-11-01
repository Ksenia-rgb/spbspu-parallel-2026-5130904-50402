#ifndef AREA_HPP
#define AREA_HPP
#include <cstddef>
#include <utility>
#include <vector>
#include "shapes.hpp"

namespace novikov
{
  using size_t_pair = std::pair< std::size_t, std::size_t >;
  using double_pair = std::pair< double, double >;
  using c_vec = std::vector< Circle >;

  c_vec readInput(Polygon &pg);
  size_t_pair calculate(const c_vec &figures, const Polygon &pg, std::size_t tries, int seed);
  std::size_t countInside(const c_vec &figures, double x, double y);
  double_pair area(const c_vec &figures, const Polygon &pg, std::size_t threads, std::size_t tries, int seed);
}

#endif
