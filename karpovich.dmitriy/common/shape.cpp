#include "shape.hpp"

namespace karpovich
{
  bool contains(const shape_t &shape, point_t point) noexcept
  {
    const double dx = point.x - shape.center.x;
    const double dy = point.y - shape.center.y;

    const double r12 = shape.r1 * shape.r1;

    if (shape.type == ShapeType::Circle)
    {
      return dx * dx + dy * dy <= r12;
    }

    const double r22 = shape.r2 * shape.r2;

    return dx * dx * r22 + dy * dy * r12 <= r12 * r22;
  }

  point_t getMinCorner(const shape_t &shape) noexcept
  {
    return {shape.center.x - shape.r1, shape.center.y - shape.r2};
  }

  point_t getMaxCorner(const shape_t &shape) noexcept
  {
    return {shape.center.x + shape.r1, shape.center.y + shape.r2};
  }
}
