#include "shapes.hpp"
#include "cmath"

namespace muh = muhamadiarov;

muh::Circle::Circle(double r, Point p) noexcept:
  r_(r),
  p_(p)
{}

bool muh::Circle::isInside(Point p) const noexcept
{
  return std::sqrt((p_.x - p.x) * (p_.x - p.x) + (p_.y - p.y) * (p_.y - p.y)) <= r_;
}

double muh::Circle::getMaxX() const noexcept
{
  return p_.x + r_;
}

double muh::Circle::getMinX() const noexcept
{
  return p_.x - r_;
}

double muh::Circle::getMaxY() const noexcept
{
  return p_.y + r_;
}

double muh::Circle::getMinY() const noexcept
{
  return p_.y - r_;
}

muh::Ellipse::Ellipse(double hr, double vr, Point p) noexcept:
  hr_(hr),
  vr_(vr),
  p_(p)
{}

bool muh::Ellipse::isInside(Point p) const noexcept
{
  return (p_.x - p.x) * (p_.x - p.x) / hr_ / hr_ + (p_.y - p.y) * (p_.y - p.y) / vr_ / vr_ <= 1;
}

double muh::Ellipse::getMaxX() const noexcept
{
  return p_.x + hr_;
}

double muh::Ellipse::getMinX() const noexcept
{
  return p_.x - hr_;
}

double muh::Ellipse::getMaxY() const noexcept
{
  return p_.y + vr_;
}

double muh::Ellipse::getMinY() const noexcept
{
  return p_.y - vr_;
}
