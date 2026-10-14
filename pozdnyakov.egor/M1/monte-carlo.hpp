#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include "arguments.hpp"
#include "shapes.hpp"

namespace pozdnyakov
{
  struct areas_t
  {
    double coverage;
    double intersection;
  };

  areas_t computeAreas(const ellipses_t& ellipses, const arguments_t& args);
}

#endif
