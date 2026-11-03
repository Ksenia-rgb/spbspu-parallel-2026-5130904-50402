#include "circle.hpp"

novikov::Circle::Circle(double r, double x, double y):
  r_(r),
  x_(x),
  y_(y)
{}

double novikov::Circle::getR() const
{
  return r_;
}

double novikov::Circle::getX() const
{
  return x_;
}

double novikov::Circle::getY() const
{
  return y_;
}
