#ifndef SHAPE_HPP
#define SHAPE_HPP

namespace karpovich
{
  struct point_t
  {
    double x;
    double y;
  };

  struct rectangle_t
  {
    point_t max;
    point_t min;
  };

  enum class ShapeType
  {
    Circle,
    Ellipse,
  };

  struct shape_t
  {
    ShapeType type;
    double r1;
    double r2;
    point_t center;
  };

  bool contains(const shape_t &shape, point_t point) noexcept;
  point_t getMinCorner(const shape_t &shape) noexcept;
  point_t getMaxCorner(const shape_t &shape) noexcept;
}

#endif
