#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <vector>

namespace shirokov
{
  struct MinMaxes
  {
    double min_x, max_x;
    double min_y, max_y;
  };

  struct Ellipse
  {
    double x, y, hr, vr;

    Ellipse(double x, double y, double hr, double vr);
    bool isInside(double px, double py) const noexcept;
    MinMaxes getMinMaxes() const noexcept;
  };

  using ellipse_list = std::vector< Ellipse >;

  MinMaxes getAllMinMaxes(const ellipse_list& ellipses);
}

#endif
