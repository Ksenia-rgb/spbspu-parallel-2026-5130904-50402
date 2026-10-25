#include "shape.hpp"

namespace karpovich
{
  bool contains(const shape_t &shape, point_t point) noexcept
  {
    const double dx = point.x - shape.center.x;
    const double dy = point.y - shape.center.y;

    const double rx2 = shape.rx * shape.rx;

    if (shape.type == shape_type::circle)
    {
      return dx * dx + dy * dy <= rx2;
    }

    const double ry2 = shape.ry * shape.ry;

    return dx * dx * ry2 + dy * dy * rx2 <= rx2 * ry2;
  }

  point_t getMinCorner(const shape_t &shape) noexcept
  {
    return {shape.center.x - shape.rx, shape.center.y - shape.ry};
  }

  point_t getMaxCorner(const shape_t &shape) noexcept
  {
    return {shape.center.x + shape.rx, shape.center.y + shape.ry};
  }
}
