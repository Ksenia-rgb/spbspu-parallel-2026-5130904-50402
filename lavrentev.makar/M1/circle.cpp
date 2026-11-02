#include "circle.hpp"

lavrentev::Circle::Circle(int r, int x, int y):
  radius_(r),
  x_(x),
  y_(y)
{}

int lavrentev::Circle::getX() const
{
  return x_;
}

int lavrentev::Circle::getY() const
{
  return y_;
}

int lavrentev::Circle::getRadius() const
{
  return radius_;
}
