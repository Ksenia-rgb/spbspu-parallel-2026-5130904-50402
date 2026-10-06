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

  Ellipse::Ellipse(double horizontalRadius, double verticalRadius, Point center) noexcept:
    horizontalRadius_(horizontalRadius),
    verticalRadius_(verticalRadius),
    center_(center)
  {}

  bool Ellipse::contains(Point point) const noexcept
  {
    const double dx = point.x - center_.x;
    const double dy = point.y - center_.y;
    const double a2 = horizontalRadius_ * horizontalRadius_;
    const double b2 = verticalRadius_ * verticalRadius_;
    return dx * dx * b2 + dy * dy * a2 <= a2 * b2;
  }

  Point Ellipse::getMinCorner() const noexcept
  {
    return {center_.x - horizontalRadius_, center_.y - verticalRadius_};
  }

  Point Ellipse::getMaxCorner() const noexcept
  {
    return {center_.x + horizontalRadius_, center_.y + verticalRadius_};
  }

}
