#ifndef SHAPES_HPP
#define SHAPES_HPP
#include <cstddef>
#include <vector>
namespace dirko {
  struct point_t {
    double x, y;
  };
  struct Circle {
    double radius;
    point_t position;
    Circle(size_t radius, point_t position);
    bool isInside(point_t p) const noexcept;
  };
  using circles_t = std::vector< Circle >;
}
#endif
