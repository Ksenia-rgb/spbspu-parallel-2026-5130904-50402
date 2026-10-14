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

  std::istream& operator>>(std::istream& in, ellipse_t& ellipse);

  bool isInside(const ellipse_t& ellipse, const point_t& point);
  rectangle_t getFrame(const ellipse_t& ellipse);
  rectangle_t getFrame(const ellipses_t& ellipses);
  double getArea(const rectangle_t& rectangle);
}

#endif
