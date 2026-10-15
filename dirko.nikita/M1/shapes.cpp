#include "shapes.hpp"
#include <cstddef>

dirko::Circle::Circle(size_t radius, point_t position):
  radius_(radius),
  position_(position)
{}
bool dirko::Circle::isInside(point_t point) const noexcept
{
  const double dx = point.x - position_.x;
  const double dy = point.y - position_.y;
  return dx * dx + dy * dy <= radius_ * radius_;
}
dirko::point_t dirko::Circle::getMaxCorner() const noexcept
{
  return {position_.x + radius_, position_.y + radius_};
}
dirko::point_t dirko::Circle::getMinCorner() const noexcept
{
  return {position_.x - radius_, position_.y - radius_};
}
dirko::Ellipse::Ellipse(double horizontal_radius, double vertical_radius, dirko::point_t position) noexcept:
  horizontal_radius_(horizontal_radius),
  vertical_radius_(vertical_radius),
  position_(position)
{}

bool dirko::Ellipse::isInside(dirko::point_t point) const noexcept
{
  const double dx = point.x - position_.x;
  const double dy = point.y - position_.y;
  const double a2 = horizontal_radius_ * horizontal_radius_;
  const double b2 = vertical_radius_ * vertical_radius_;
  return dx * dx * b2 + dy * dy * a2 <= a2 * b2;
}

dirko::point_t dirko::Ellipse::getMinCorner() const noexcept
{
  return {position_.x - horizontal_radius_, position_.y - vertical_radius_};
}

dirko::point_t dirko::Ellipse::getMaxCorner() const noexcept
{
  return {position_.x + horizontal_radius_, position_.y + vertical_radius_};
}
