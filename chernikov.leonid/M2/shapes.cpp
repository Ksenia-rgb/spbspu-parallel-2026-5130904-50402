#include "shapes.hpp"
#include <algorithm>
#include <memory>
#include <ostream>
#include <stdexcept>

chernikov::Circle::Circle(double radius, const point_t &c):
  r(radius),
  center(c)
{}

bool chernikov::Circle::contains(const point_t &p) const
{
  const double dx = p.x - center.x;
  const double dy = p.y - center.y;
  return dx * dx + dy * dy <= r * r;
}

chernikov::frame_t chernikov::Circle::getFrame() const
{
  return {{center.x - r, center.y - r}, {center.x + r, center.y + r}};
}

void chernikov::Circle::print(std::ostream &out) const
{
  out << r << " (" << center.x << " " << center.y << ")";
}

chernikov::shape_data_t chernikov::Circle::toData() const
{
  return {0, r, r, center.x, center.y, 0.0};
}

std::unique_ptr< chernikov::Shape > chernikov::makeShape(const shape_data_t &data)
{
  if (data.type == 0) {
    return std::unique_ptr< Shape >(new Circle(data.rx, {data.x, data.y}));
  }
  throw std::invalid_argument("unknown shape type");
}

chernikov::frame_t chernikov::unite(const frame_t &a, const frame_t &b)
{
  frame_t res = a;
  res.min.x = std::min(res.min.x, b.min.x);
  res.min.y = std::min(res.min.y, b.min.y);
  res.max.x = std::max(res.max.x, b.max.x);
  res.max.y = std::max(res.max.y, b.max.y);
  return res;
}

void chernikov::printFrame(std::ostream &out, const frame_t &frame)
{
  out << "(" << frame.min.x << " " << frame.min.y << ") (" << frame.max.x << " " << frame.max.y << ")";
}
