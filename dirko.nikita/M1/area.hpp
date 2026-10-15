#ifndef AREA_HPP
#define AREA_HPP
#include <cstddef>
#include <utility>
#include <vector>
#include <memory>
#include "shapes.hpp"
namespace dirko {
  using shapes_t = std::vector< std::unique_ptr< Shape > >;
  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed);
}
#endif
