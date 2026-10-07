#ifndef CALCULATIONS_HPP
#define CALCULATIONS_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include "circle.hpp"
#include "polygon.hpp"

namespace lavrentev
{
  std::vector< lavrentev::Circle > readInput(lavrentev::Polygon &pg);
  std::pair< std::size_t, std::size_t > calculate(const std::vector< lavrentev::Circle > &figures,
                                                  const lavrentev::Polygon &pg,
                                                  std::size_t tries,
                                                  int seed);
  std::size_t countInside(const std::vector< lavrentev::Circle > &figures, double x, double y);
  std::pair< double, double > area(const std::vector< lavrentev::Circle > &figures,
                                   const lavrentev::Polygon &pg,
                                   std::size_t threads,
                                   std::size_t tries,
                                   int seed);
}

#endif