#include "shapes.hpp"
#include <future>
#include <random>
#include <iostream>
#include <limits>

namespace vishnevskiy
{
  bool isInside(double x, double y, const Circle& c)
  {
    double rx = x - c.x;
    double ry = y - c.y;
    double r  = c.r;
    return (rx * rx + ry * ry) <= (r * r);
  }

  CircleBoundaries getBoundaries(std::vector<Circle>& circles)
  {
    CircleBoundaries b
    {
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest(),
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest()
    };
    for (size_t i = 0; i < circles.size(); ++i)
    {
      const Circle& c = circles[i];
      double curMinX = c.x - c.r;
      double curMaxX = c.x + c.r;
      double curMinY = c.y - c.r;
      double curMaxY = c.y + c.r;
      if (curMinX < b.minX)
      {
        b.minX = curMinX;
      }
      if (curMaxX > b.maxX)
      {
        b.maxX = curMaxX;
      }
      if (curMinY < b.minY)
      {
        b.minY = curMinY;
      }
      if (curMaxY > b.maxY)
      {
        b.maxY = curMaxY;
      }
    }
    return b;
  }

  void generatePoint(std::mt19937_64& gen, const CircleBoundaries& b, double& px, double& py)
  {
    std::uniform_real_distribution<double> distX(static_cast<double>(b.minX), static_cast<double>(b.maxX));
    std::uniform_real_distribution<double> distY(static_cast<double>(b.minY), static_cast<double>(b.maxY));
    px = distX(gen);
    py = distY(gen);
  }

  size_t calcUnion(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b)
  {
    std::mt19937_64 gen(seed);
    size_t count = 0;
    for (size_t i = 0; i < tests; ++i)
    {
      double x = 0.0;
      double y = 0.0;
      generatePoint(gen, b, x, y);
      for (size_t j = 0; j < circles.size(); ++j)
      {
        if (isInside(x, y, circles[j]))
        {
          count++;
          break;
        }
      }
    }
    return count;
  }

  size_t calcIntersection(const std::vector<Circle>& circles, size_t tests, size_t seed, const CircleBoundaries& b)
  {
    std::mt19937_64 gen(seed);
    size_t count = 0;
    for (size_t i = 0; i < tests; ++i)
    {
      double x = 0.0;
      double y = 0.0;
      generatePoint(gen, b, x, y);
      bool insideAll = true;
      for (size_t j = 0; j < circles.size(); ++j)
      {
        if (!isInside(x, y, circles[j]))
        {
          insideAll = false;
          break;
        }
      }
      if (insideAll)
      {
        count++;
      }
    }
    return count;
  }

  size_t runUnion(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b)
  {
    size_t block = tests / threads;
    size_t remainder = tests % threads;
    std::vector<std::future<size_t>> futures;
    futures.reserve(threads);

    for (size_t i = 0; i < threads; ++i)
    {
      size_t currTests = block;
      if (i < remainder)
      {
        currTests++;
      }
      futures.push_back(std::async(std::launch::async, calcUnion, circles, currTests, seed, b));
    }

    size_t res = 0;
    for (size_t i = 0; i < futures.size(); ++i)
    {
      res += futures[i].get();
    }
    return res;
  }

  size_t runIntersection(std::vector<Circle>& circles, size_t threads, size_t tests, size_t seed, CircleBoundaries& b)
  {
    size_t block = tests / threads;
    size_t remainder = tests % threads;
    std::vector<std::future<size_t>> futures;
    futures.reserve(threads);

    for (size_t i = 0; i < threads; ++i)
    {
      size_t currTests = block;
      if (i < remainder)
      {
        currTests++;
      }
      futures.push_back(std::async(std::launch::async, calcIntersection, circles, currTests, seed, b));
    }

    size_t res = 0;
    for (size_t i = 0; i < futures.size(); ++i)
    {
      res += futures[i].get();
    }
    return res;
  }
}
