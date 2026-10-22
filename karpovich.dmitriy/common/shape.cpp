#include "shape.hpp"

namespace karpovich
{
  Circle::Circle(double radius, point_t center) noexcept:
    radius_(radius),
    center_(center)
  {}

  bool Circle::contains(point_t point) const noexcept
  {
    const double dx = point.x - center_.x;
    const double dy = point.y - center_.y;
    return dx * dx + dy * dy <= radius_ * radius_;
  }

  point_t Circle::getMinCorner() const noexcept
  {
    return {center_.x - radius_, center_.y - radius_};
  }

  point_t Circle::getMaxCorner() const noexcept
  {
    return {center_.x + radius_, center_.y + radius_};
  }

  Ellipse::Ellipse(double horizontal_radius, double vertical_radius, point_t center) noexcept:
    horizontal_radius_(horizontal_radius),
    vertical_radius_(vertical_radius),
    center_(center)
  {}

  bool Ellipse::contains(point_t point) const noexcept
  {
    const double dx = point.x - center_.x;
    const double dy = point.y - center_.y;
    const double a2 = horizontal_radius_ * horizontal_radius_;
    const double b2 = vertical_radius_ * vertical_radius_;
    return dx * dx * b2 + dy * dy * a2 <= a2 * b2;
  }

  point_t Ellipse::getMinCorner() const noexcept
  {
    return {center_.x - horizontal_radius_, center_.y - vertical_radius_};
  }

  point_t Ellipse::getMaxCorner() const noexcept
  {
    return {center_.x + horizontal_radius_, center_.y + vertical_radius_};
  }

  double Circle::getRadius() const noexcept
  {
    return radius_;
  }

  point_t Circle::getCenter() const noexcept
  {
    return center_;
  }

  double Ellipse::getHorizontalRadius() const noexcept
  {
    return horizontal_radius_;
  }

  double Ellipse::getVerticalRadius() const noexcept
  {
    return vertical_radius_;
  }

  point_t Ellipse::getCenter() const noexcept
  {
    return center_;
  }

}
