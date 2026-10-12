#ifndef SHAPES_H
#define SHAPES_H

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
    double minX;
    double maxX;
    double minY;
    double maxY;

    double width()
    {
      return maxX - minX;
    }
    double height()
    {
      return maxY - minY;
    }
    double area()
    {
      return width() * height();
    }
  };

  bool isInside(const double x, const double y, const Circle& c);
  void generatePoint(std::mt19937_64& gen, const CircleBoundaries& b, double& px, double& py);
  CircleBoundaries getBoundaries(std::vector<Circle>& circles);
  size_t calcUnion(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t calcIntersection(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b);
  size_t runUnion(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
  size_t runIntersection(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b);
}
#endif
