#include "shapes.hpp"
#include <algorithm>
#include <cmath>
#include <istream>

std::istream& pozdnyakov::operator>>(std::istream& in, ellipse_t& ellipse)
{
  int horizontal = 0;
  int vertical = 0;
  int x = 0;
  int y = 0;
  if (in >> horizontal >> vertical >> x >> y)
  {
    ellipse.center.x = x;
    ellipse.center.y = y;
    ellipse.horizontal_radius = horizontal;
    ellipse.vertical_radius = (vertical != 0) ? vertical : horizontal;
  }
  return in;
}

bool pozdnyakov::isInside(const ellipse_t& ellipse, const point_t& point)
{
  const double dx = (point.x - ellipse.center.x) / ellipse.horizontal_radius;
  const double dy = (point.y - ellipse.center.y) / ellipse.vertical_radius;
  return (dx * dx + dy * dy) <= 1.0;
}

pozdnyakov::rectangle_t pozdnyakov::getFrame(const ellipse_t& ellipse)
{
  const double width = std::abs(ellipse.horizontal_radius);
  const double height = std::abs(ellipse.vertical_radius);
  const point_t left_bottom{ellipse.center.x - width, ellipse.center.y - height};
  const point_t right_top{ellipse.center.x + width, ellipse.center.y + height};
  return {left_bottom, right_top};
}

pozdnyakov::rectangle_t pozdnyakov::getFrame(const ellipses_t& ellipses)
{
  const rectangle_t first = getFrame(ellipses.front());
  double low = std::min(first.left_bottom.x, first.left_bottom.y);
  double high = std::max(first.right_top.x, first.right_top.y);
  for (const ellipse_t& ellipse : ellipses)
  {
    const rectangle_t next = getFrame(ellipse);
    low = std::min({low, next.left_bottom.x, next.left_bottom.y});
    high = std::max({high, next.right_top.x, next.right_top.y});
  }
  return {{low, low}, {high, high}};
}

double pozdnyakov::getArea(const rectangle_t& rectangle)
{
  return (rectangle.right_top.x - rectangle.left_bottom.x) * (rectangle.right_top.y - rectangle.left_bottom.y);
}
