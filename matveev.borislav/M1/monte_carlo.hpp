#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <vector>
#include "figure.hpp"
#include "options.hpp"

namespace matveev
{
  struct areas_t
  {
    long double coverage;
    long double intersection;
  };

  areas_t calculateAreas(const std::vector< figure_t >& figures, const options_t& options);
}

#endif
