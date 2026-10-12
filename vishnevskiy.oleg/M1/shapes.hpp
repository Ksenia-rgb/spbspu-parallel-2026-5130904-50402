#ifndef SHAPES_H
#define SHAPES_H

#include <cstddef>
#include <vector>
#include <random>

namespace vishnevskiy
{
  struct Circle
  {
    long long r, x, y;
  };

  struct CircleBoundaries
  {
    long long minX;
    long long maxX;
    long long minY;
    long long maxY;

    long long width()
    {
      return maxX - minX;
    }
    long long height()
    {
      return maxY - minY;
    }
    long long area()
    {
      return width() * height();
    }
  };

  bool isInside(const long long x, const long long y, const Circle& c);
  void generatePoint(std::mt19937_64& gen, const CircleBoundaries& b, double& px, double& py);
  CircleBoundaries getBoundaries(std::vector<Circle>& circles);
  size_t calcUnion(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t calcIntersection(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t runUnion(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
  size_t runIntersection(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
}
#endif
