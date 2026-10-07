#ifndef CALCULATIONS_HPP
#define CALCULATIONS_HPP

#include "polygon.hpp"

#include <vector>
#include <utility>
#include "circle.hpp"

namespace lavrentev
{
  std::vector< lavrentev::Circle > readInput(lavrentev::Polygon &pg);
  std::pair< size_t, size_t > calculate(
    const std::vector< lavrentev::Circle > &figures,
    const lavrentev::Polygon &pg,
    size_t tries,
    int seed
  );
  size_t countInside(const std::vector< lavrentev::Circle > &figures, double x, double y);
  std::pair< double, double > area(
    const std::vector< lavrentev::Circle > &figures,
    const lavrentev::Polygon &pg,
    size_t threads,
    size_t tries,
    int seed
  );
}

#endif
