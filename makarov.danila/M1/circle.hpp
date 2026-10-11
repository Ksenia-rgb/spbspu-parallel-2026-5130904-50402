#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <istream>
#include <vector>

namespace makarov {
  struct Circle {
    double r;
    double x;
    double y;
  };

  std::istream& operator>>(std::istream& in, Circle& circle);
  std::vector< Circle > readCircles(std::istream& in);
}

#endif
