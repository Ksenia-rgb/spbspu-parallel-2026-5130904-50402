#include "shapes.hpp"

bool dirko::contains(const shape_t &shape, point_t point) noexcept
{
  const double dx = point.x - shape.position.x;
  const double dy = point.y - shape.position.y;

  const double rx2 = shape.rx * shape.rx;

  if (shape.type == shapeType::CIRCLE) {
    return dx * dx + dy * dy <= rx2;
  }

  const double ry2 = shape.ry * shape.ry;

  return dx * dx * ry2 + dy * dy * rx2 <= rx2 * ry2;
}

dirko::point_t dirko::getMinCorner(const shape_t &shape) noexcept
{
  return {shape.position.x - shape.rx, shape.position.y - shape.ry};
}

dirko::point_t dirko::getMaxCorner(const shape_t &shape) noexcept
{
  return {shape.position.x + shape.rx, shape.position.y + shape.ry};
}
