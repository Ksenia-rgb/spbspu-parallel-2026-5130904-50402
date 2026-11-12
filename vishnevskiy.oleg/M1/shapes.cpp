#include "shapes.hpp"
#include <future>
#include <random>
#include <iostream>
#include <limits>

namespace vishnevskiy
{
 constexpr double mx = std::numeric_limits< double >::max();
 constexpr double mn = std::numeric_limits< double >::lowest();
  bool isInside(double x, double y, const Circle& c)
  {
    const double rx = x - c.x;
    const double ry = y - c.y;
    const double r = c.r;
    return (rx * rx + ry * ry) <= (r * r);
  }

  CircleBoundaries getBoundaries(std::vector< Circle >& circles)
  {
    CircleBoundaries b{mx, mn, mx, mn};
    for (size_t i = 0; i < circles.size(); ++i)
    {
      const Circle& c = circles[i];
      const double cur_min_x = c.x - c.r;
      const double cur_max_x = c.x + c.r;
      const double cur_min_y = c.y - c.r;
      const double cur_max_y = c.y + c.r;
      if (cur_min_x < b.min_x)
      {
        b.min_x = cur_min_x;
      }
      if (cur_max_x > b.max_x)
      {
        b.max_x = cur_max_x;
      }
      if (cur_min_y < b.min_y)
      {
        b.min_y = cur_min_y;
      }
      if (cur_max_y > b.max_y)
      {
        b.max_y = cur_max_y;
      }
    }
    return b;
  }

  void generatePoint(std::default_random_engine& gen, const CircleBoundaries& b, double& px, double& py)
  {
    std::uniform_real_distribution< double > dist_x(static_cast< double >(b.min_x), static_cast< double >(b.max_x));
    std::uniform_real_distribution< double > dist_y(static_cast< double >(b.min_y), static_cast< double >(b.max_y));
    px = dist_x(gen);
    py = dist_y(gen);
  }

  size_t calcUnion(const std::vector< Circle >& circles, long long tests, long long seed, const CircleBoundaries& b)
  {
    std::default_random_engine gen(seed);
    size_t count = 0;
    for (long long i = 0; i < tests; ++i)
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

  size_t calcIntersection(
      const std::vector< Circle >& circles, long long tests, long long seed, const CircleBoundaries& b)
  {
    std::default_random_engine gen(seed);
    size_t count = 0;
    for (long long i = 0; i < tests; ++i)
    {
      double x = 0.0;
      double y = 0.0;
      generatePoint(gen, b, x, y);
      bool inside_all = true;
      for (size_t j = 0; j < circles.size(); ++j)
      {
        if (!isInside(x, y, circles[j]))
        {
          inside_all = false;
          break;
        }
      }
      if (inside_all)
      {
        count++;
      }
    }
    return count;
  }

  size_t runUnion(
      std::vector< Circle >& circles, long long threads, long long tests, long long seed, CircleBoundaries& b)
  {
    const long long block = tests / threads;
    const long long remainder = tests % threads;
    std::vector< std::future< size_t > > futures;
    futures.reserve(threads);

    for (long long i = 0; i < threads; ++i)
    {
      long long curr_tests = block;
      if (i < remainder)
      {
        curr_tests++;
      }
      futures.push_back(std::async(std::launch::async, calcUnion, circles, curr_tests, seed + i, b));
    }

    size_t res = 0;
    for (size_t i = 0; i < futures.size(); ++i)
    {
      res += futures[i].get();
    }
    return res;
  }

  size_t runIntersection(
      std::vector< Circle >& circles, long long threads, long long tests, long long seed, CircleBoundaries& b)
  {
    const long long block = tests / threads;
    const long long remainder = tests % threads;
    std::vector< std::future< size_t > > futures;
    futures.reserve(threads);

    for (long long i = 0; i < threads; ++i)
    {
      long long curr_tests = block;
      if (i < remainder)
      {
        curr_tests++;
      }
      futures.push_back(std::async(std::launch::async, calcIntersection, circles, curr_tests, seed + i, b));
    }

    size_t res = 0;
    for (size_t i = 0; i < futures.size(); ++i)
    {
      res += futures[i].get();
    }
    return res;
  }
}
