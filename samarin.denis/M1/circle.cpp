#include "circle.hpp"
#include <stdexcept>

samarin::Circle::Circle(double radius, point_t center):
  radius_(radius),
  center_(center)
{
  if (radius <= 0.0) {
    throw std::invalid_argument("Circle radius must be positive");
  }
}

bool samarin::Circle::contains(point_t point) const noexcept
{
  const double dx = point.x - center_.x;
  const double dy = point.y - center_.y;
  return (dx * dx) + (dy * dy) <= radius_ * radius_;
}

samarin::frame_t samarin::Circle::getFrame() const noexcept
{
  return {{center_.x - radius_, center_.y - radius_}, {center_.x + radius_, center_.y + radius_}};
}
