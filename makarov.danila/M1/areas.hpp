#ifndef AREAS_HPP
#define AREAS_HPP

#include <cstddef>
#include <vector>
#include "circle.hpp"

namespace makarov {
  struct Areas {
    double any;
    double all;
  };

  Areas getAreas(const std::vector< Circle >& circles, std::size_t threads, std::size_t tries, std::size_t seed);
}

#endif
