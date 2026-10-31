#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <iosfwd>
#include <vector>

namespace pozdnyakov
{
  struct point_t
  {
    double x;
    double y;
  };

  struct ellipse_t
  {
    point_t center;
    double horizontal_radius;
    double vertical_radius;
  };

  struct rectangle_t
  {
    point_t left_bottom;
    point_t right_top;
  };

  using ellipses_t = std::vector< ellipse_t >;

  ellipse_t makeCircle();

}

#endif
