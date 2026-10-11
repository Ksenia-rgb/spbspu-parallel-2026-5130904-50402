#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include "circle.hpp"

namespace makarov
{
  struct Box
  {
    double left;
    double right;
    double bottom;
    double top;
  };

  bool isInside(const Circle& c, double x, double y);
  Box getBox(const std::vector< Circle >& circles);
  double getArea(const Box& box);
}

#endif
