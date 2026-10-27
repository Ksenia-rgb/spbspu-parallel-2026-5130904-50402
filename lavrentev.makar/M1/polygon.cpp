#include "polygon.hpp"

lavrentev::Polygon::Polygon(int x1, int y1, int x2, int y2):
  min_x_(x1),
  min_y_(y1),
  max_x_(x2),
  max_y_(y2)
{}

int lavrentev::Polygon::getMaxX() const
{
  return max_x_;
}

int lavrentev::Polygon::getMaxY() const
{
  return max_y_;
}

int lavrentev::Polygon::getMinX() const
{
  return min_x_;
}

int lavrentev::Polygon::getMinY() const
{
  return min_y_;
}

void lavrentev::Polygon::setMaxX(int x)
{
  max_x_ = x;
}

void lavrentev::Polygon::setMaxY(int y)
{
  max_y_ = y;
}

void lavrentev::Polygon::setMinX(int x)
{
  min_x_ = x;
}

void lavrentev::Polygon::setMinY(int y)
{
  min_y_ = y;
}
