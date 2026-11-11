#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <cstddef>
#include <vector>
#include "circle.hpp"
#include "geometry.hpp"

namespace makarov
{
  struct Hits
  {
    std::size_t any;
    std::size_t all;
  };

  Hits countHits(const std::vector< Circle >& circles, const Box& box, std::size_t tries, std::size_t seed);
}

#endif
