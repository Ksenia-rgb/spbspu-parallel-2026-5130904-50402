#include "geometry.hpp"
#include <algorithm>
#include <vector>
#include "circle.hpp"

bool makarov::isInside(const Circle& c, double x, double y)
{
  const double dx = x - c.x;
  const double dy = y - c.y;
  return dx * dx + dy * dy <= c.r * c.r;
}

makarov::Box makarov::getBox(const std::vector< Circle >& circles)
{
  if (circles.empty())
  {
    return Box{0.0, 0.0, 0.0, 0.0};
  }
  const Circle& first = circles.front();
  Box box{first.x - first.r, first.x + first.r, first.y - first.r, first.y + first.r};
  for (const Circle& c : circles)
  {
    box.left = std::min(box.left, c.x - c.r);
    box.right = std::max(box.right, c.x + c.r);
    box.bottom = std::min(box.bottom, c.y - c.r);
    box.top = std::max(box.top, c.y + c.r);
  }
  return box;
}

double makarov::getArea(const Box& box)
{
  return (box.right - box.left) * (box.top - box.bottom);
}
