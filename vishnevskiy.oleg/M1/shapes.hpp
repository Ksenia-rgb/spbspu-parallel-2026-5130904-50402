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
    double min_x;
    double max_x;
    double min_y;
    double max_y;

    double width() const
    {
      return max_x - min_x;
    }
    double height() const
    {
      return max_y - min_y;
    }
    double area() const
    {
      return width() * height();
    }
  };

  bool isInside(double x, double y, const Circle& c);
  void generatePoint(std::default_random_engine& gen, const CircleBoundaries& b, double& px, double& py);
  CircleBoundaries getBoundaries(std::vector< Circle >& circles);
  size_t calcUnion(const std::vector< Circle >& circles, long long tests, long long seed, const CircleBoundaries& b);
  size_t calcIntersection(const std::vector< Circle >& circles,
    long long tests, long long seed, const CircleBoundaries& b);
  size_t runUnion(std::vector< Circle >& circles,
    long long threads, long long tests, long long seed, CircleBoundaries& b);
  size_t runIntersection(std::vector< Circle >& circles,
    long long threads, long long tests, long long seed, CircleBoundaries& b);
}
#endif
