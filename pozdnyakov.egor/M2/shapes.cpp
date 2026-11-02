#include "shapes.hpp"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>

namespace
{
  std::ostream& printPoint(std::ostream& out, const pozdnyakov::point_t& point)
  {
    return out << '(' << point.x << ' ' << point.y << ')';
  }
}

std::ostream& pozdnyakov::operator<<(std::ostream& out, const ellipse_t& ellipse)
{
  out << ellipse.horizontal_radius << ' ';
  return printPoint(out, ellipse.center);
}

std::ostream& pozdnyakov::operator<<(std::ostream& out, const rectangle_t& rectangle)
{
  printPoint(out, rectangle.left_bottom) << ' ';
  return printPoint(out, rectangle.right_top);
}

pozdnyakov::ellipse_t pozdnyakov::makeCircle(double radius, const point_t& center)
{
  if (radius <= 0.0)
  {
    throw std::invalid_argument("radius must be positive");
  }
  return {center, radius, radius};
}

bool pozdnyakov::isInside(const ellipse_t& ellipse, const point_t& point)
{
  const double dx = (point.x - ellipse.center.x) / ellipse.horizontal_radius;
  const double dy = (point.y - ellipse.center.y) / ellipse.vertical_radius;
  return ((dx * dx) + (dy * dy)) <= 1.0;
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
  if (ellipses.empty())
  {
    throw std::logic_error("frame of empty set is undefined");
  }
  rectangle_t frame = getFrame(ellipses.front());
  for (const ellipse_t& ellipse : ellipses)
  {
    const rectangle_t next = getFrame(ellipse);
    frame.left_bottom.x = std::min(frame.left_bottom.x, next.left_bottom.x);
    frame.left_bottom.y = std::min(frame.left_bottom.y, next.left_bottom.y);
    frame.right_top.x = std::max(frame.right_top.x, next.right_top.x);
    frame.right_top.y = std::max(frame.right_top.y, next.right_top.y);
  }
  return frame;
}

double pozdnyakov::getArea(const rectangle_t& rectangle)
{
  return (rectangle.right_top.x - rectangle.left_bottom.x) * (rectangle.right_top.y - rectangle.left_bottom.y);
}
