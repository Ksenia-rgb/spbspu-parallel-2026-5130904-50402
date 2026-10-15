#include "ellipse.hpp"
#include <stdexcept>

samarin::Ellipse::Ellipse(double horizontalRadius, double verticalRadius, point_t center):
  horizontalRadius_(horizontalRadius),
  verticalRadius_(verticalRadius),
  center_(center)
{
  if (horizontalRadius <= 0.0 || verticalRadius <= 0.0) {
    throw std::invalid_argument("Ellipse radii must be positive");
  }
}

bool samarin::Ellipse::contains(point_t point) const noexcept
{
  const double dx = point.x - center_.x;
  const double dy = point.y - center_.y;
  const double a2 = horizontalRadius_ * horizontalRadius_;
  const double b2 = verticalRadius_ * verticalRadius_;
  return (dx * dx * b2) + (dy * dy * a2) <= a2 * b2;
}

samarin::frame_t samarin::Ellipse::getFrame() const noexcept
{
  const point_t min{center_.x - horizontalRadius_, center_.y - verticalRadius_};
  const point_t max{center_.x + horizontalRadius_, center_.y + verticalRadius_};
  return {min, max};
}
