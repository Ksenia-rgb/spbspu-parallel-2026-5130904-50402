#ifndef MATVEEV_SHAPE_HPP
#define MATVEEV_SHAPE_HPP

#include <iosfwd>
#include <vector>

namespace matveev
{
  struct shape_t
  {
    long long radiusX = 0;
    long long radiusY = 0;
    long long x = 0;
    long long y = 0;
    double angle = 0.0;
    bool ellipse = false;
  };

  struct frame_t
  {
    long double left = 0.0L;
    long double bottom = 0.0L;
    long double right = 0.0L;
    long double top = 0.0L;
  };

  bool contains(const shape_t &shape, double x, double y);
  frame_t getFrame(const shape_t &shape);
  frame_t getFrame(const std::vector< shape_t > &shapes);
  std::vector< shape_t > translateShapes(const std::vector< shape_t > &shapes);
  void printShape(std::ostream &out, const shape_t &shape);
  void printFrame(std::ostream &out, const frame_t &frame);
}

#endif
