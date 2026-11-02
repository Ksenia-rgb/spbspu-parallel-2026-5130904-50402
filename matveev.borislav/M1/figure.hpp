#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <iosfwd>
#include <vector>

namespace matveev
{
  struct figure_t
  {
    long double horizontal_radius;
    long double vertical_radius;
    long double center_x;
    long double center_y;
  };

  struct bounds_t
  {
    long double min_x;
    long double max_x;
    long double min_y;
    long double max_y;
  };

  std::vector< figure_t > readFigures(std::istream& input);
  bool isInside(const figure_t& figure, long double x, long double y);
  bounds_t findBounds(const std::vector< figure_t >& figures);
}

#endif
