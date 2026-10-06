#include "shape.hpp"

namespace karpovich
{
  Circle::Circle(double radius, Point center) noexcept:
    radius_(radius),
    center_(center)
  {}

  bool Circle::contains(Point point) const noexcept
  {
    const double dx = point.x - center_.x;
    const double dy = point.y - center_.y;
    return dx * dx + dy * dy <= radius_ * radius_;
  }

  Point Circle::getMinCorner() const noexcept
  {
    return {center_.x - radius_, center_.y - radius_};
  }

  Point Circle::getMaxCorner() const noexcept
  {
    return {center_.x + radius_, center_.y + radius_};
  }

  Ellipse::Ellipse(double horizontal_radius, double vertical_radius, Point center) noexcept:
    horizontal_radius_(horizontal_radius),
    vertical_radius_(vertical_radius),
    center_(center)
  {}

  bool Ellipse::contains(Point point) const noexcept
  {
    const double dx = point.x - center_.x;
    const double dy = point.y - center_.y;
    const double a2 = horizontal_radius_ * horizontal_radius_;
    const double b2 = vertical_radius_ * vertical_radius_;
    return dx * dx * b2 + dy * dy * a2 <= a2 * b2;
  }

  Point Ellipse::getMinCorner() const noexcept
  {
    return {center_.x - horizontal_radius_, center_.y - vertical_radius_};
  }

  Point Ellipse::getMaxCorner() const noexcept
  {
    return {center_.x + horizontal_radius_, center_.y + vertical_radius_};
  }

}
