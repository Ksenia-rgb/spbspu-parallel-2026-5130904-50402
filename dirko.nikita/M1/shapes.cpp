#include "shapes.hpp"
#include <cstddef>

dirko::Circle::Circle(size_t radius, point_t position):
  radius(radius),
  position(position)
{}
bool dirko::Circle::isInside(point_t p) const noexcept
{
  return (p.x - position.x) * (p.x - position.x) + (p.y - position.y) * (p.y - position.y) <= radius * radius;
}
