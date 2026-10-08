#include "Shape.hpp"
#include <algorithm>

namespace afanasev
{
  Shape::Shape(long long r, long long, long long cx, long long cy)
  {
    r_ = static_cast< double >(r);
    x_ = static_cast< double >(cx);
    y_ = static_cast< double >(cy);
  }

  void Shape::extendBBox(double & min_x, double & max_x, double & min_y, double & max_y) const noexcept
  {
    min_x = std::min(min_x, x_ - r_);
    max_x = std::max(max_x, x_ + r_);
    min_y = std::min(min_y, y_ - r_);
    max_y = std::max(max_y, y_ + r_);
  }

  bool Shape::contains(double px, double py) const noexcept
  {
    const double dx = px - x_;
    const double dy = py - y_;
    return dx * dx + dy * dy <= r_ * r_;
  }
}
