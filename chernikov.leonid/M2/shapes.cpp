#include "shapes.hpp"
#include <algorithm>
#include <memory>
#include <ostream>

chernikov::Circle::Circle(double radius, const point_t &c):
  r(radius),
  center(c)
{}

// [M1] бывший isInside
bool chernikov::Circle::contains(const point_t &p) const
{
  const double dx = p.x - center.x;
  const double dy = p.y - center.y;
  return dx * dx + dy * dy <= r * r;
}

// [M1] кусок getShapesFrame: рамка одного круга
chernikov::frame_t chernikov::Circle::getFrame() const
{
  return {{center.x - r, center.y - r}, {center.x + r, center.y + r}};
}

void chernikov::Circle::print(std::ostream &out) const
{
  // TODO
  (void)out;
}

chernikov::shape_data_t chernikov::Circle::toData() const
{
  // TODO
  return {};
}

std::unique_ptr< chernikov::Shape > chernikov::makeShape(const shape_data_t &data)
{
  // TODO: по data.type создать нужную фигуру
  (void)data;
  return nullptr;
}

// [M1] кусок getShapesFrame: объединение двух рамок
chernikov::frame_t chernikov::unite(const frame_t &a, const frame_t &b)
{
  frame_t res = a;
  res.min.x = std::min(res.min.x, b.min.x);
  res.min.y = std::min(res.min.y, b.min.y);
  res.max.x = std::max(res.max.x, b.max.x);
  res.max.y = std::max(res.max.y, b.max.y);
  return res;
}