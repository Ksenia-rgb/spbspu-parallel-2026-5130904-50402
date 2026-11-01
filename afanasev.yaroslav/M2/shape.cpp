#include "shape.hpp"

namespace afanasev
{
  Shape Shape::makeCircle(long long r, long long x, long long y)
  {
    Shape s;
    s.r_ = r;
    s.x_ = x;
    s.y_ = y;
    return s;
  }

  bbox_t Shape::bbox() const noexcept
  {
    return {x_ - r_, y_ - r_, x_ + r_, y_ + r_};
  }
}
