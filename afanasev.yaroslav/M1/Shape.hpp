#ifndef SHAPE_HPP
#define SHAPE_HPP

namespace afanasev
{
  class Shape
  {
  public:
    Shape(long long r, long long, long long cx, long long cy);

    void extendBBox(double & min_x, double & max_x, double & min_y, double & max_y) const noexcept;
    bool contains(double px, double py) const noexcept;

  private:
    double r_ = 0.0;
    double x_ = 0.0;
    double y_ = 0.0;
  };
}

#endif
