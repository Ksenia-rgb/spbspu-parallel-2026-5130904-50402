#include "shapes.hpp"

novikov::Circle::Circle(int r, int x, int y):
  radius_(r),
  x_(x),
  y_(y)
{}

int novikov::Circle::getX() const
{
  return x_;
}

int novikov::Circle::getY() const
{
  return y_;
}

int novikov::Circle::getRadius() const
{
  return radius_;
}

novikov::Polygon::Polygon(int x1, int y1, int x2, int y2):
  min_x_(x1),
  min_y_(y1),
  max_x_(x2),
  max_y_(y2)
{}

int novikov::Polygon::getMaxX() const
{
  return max_x_;
}

int novikov::Polygon::getMaxY() const
{
  return max_y_;
}

int novikov::Polygon::getMinX() const
{
  return min_x_;
}

int novikov::Polygon::getMinY() const
{
  return min_y_;
}

void novikov::Polygon::setMaxX(int x)
{
  max_x_ = x;
}

void novikov::Polygon::setMaxY(int y)
{
  max_y_ = y;
}

void novikov::Polygon::setMinX(int x)
{
  min_x_ = x;
}

void novikov::Polygon::setMinY(int y)
{
  min_y_ = y;
}
