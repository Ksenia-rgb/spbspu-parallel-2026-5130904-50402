#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <cstddef>
#include <vector>
#include <random>

namespace vishnevskiy
{
  struct Circle
  {
    double r, x, y;
  };

  struct CircleBoundaries
  {
    const double min_x;
    const double max_x;
    const double min_y;
    const double max_y;

    const double width()
    {
      return max_x - min_x;
    }
    const double height()
    {
      return max_y - min_y;
    }
    const double area()
    {
      return width() * height();
    }
  };

  bool isInside(double x, double y, const Circle& c);
  void generatePoint(std::mt19937_64& gen, const CircleBoundaries& b, double& px, double& py);
  CircleBoundaries getBoundaries(std::vector< Circle >& circles);
  size_t calcUnion(const std::vector< Circle >& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t calcIntersection(const std::vector< Circle >& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t runUnion(std::vector< Circle >& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
  size_t runIntersection(std::vector< Circle >& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
}
#endif
