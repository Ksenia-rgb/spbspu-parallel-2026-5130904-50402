#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <ostream>

namespace afanasev
{
  struct point_t
  {
    long long x = 0;
    long long y = 0;
  };

  struct bbox_t
  {
    long long min_x = 0;
    long long min_y = 0;
    long long max_x = 0;
    long long max_y = 0;
  };

  inline std::ostream & operator<<(std::ostream & out, const point_t & p)
  {
    return out << "(" << p.x << " " << p.y << ")";
  }

  class Shape
  {
  public:
    static Shape makeCircle(long long r, long long x, long long y);

    long long radius() const noexcept
    {
      return r_;
    }
    point_t center() const noexcept
    {
      return {x_, y_};
    }

    bbox_t bbox() const noexcept;

  private:
    Shape() = default;

    long long r_ = 1;
    long long x_ = 0;
    long long y_ = 0;
  };
}

#endif
