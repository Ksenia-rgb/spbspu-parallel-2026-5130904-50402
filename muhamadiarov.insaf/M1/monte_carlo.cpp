#include "monte_carlo.hpp"
#include <algorithm>
#include <future>
#include <random>
#include <cstddef>
#include <utility>
#include <vector>
#include <functional>

namespace muhamadiarov
{
  std::pair< size_t, size_t > getCountOfPoints(const shape_t &shapes, const size_t tries, const size_t seed,
      const double x_max, const double x_min, const double y_max, const double y_min)
  {
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > dist_x(x_min, x_max);
    std::uniform_real_distribution< double > dist_y(y_min, y_max);
    std::pair< size_t, size_t > results{0, 0};
    for (size_t i = 0; i < tries; ++i)
    {
      const double x = dist_x(gen);
      const double y = dist_y(gen);
      size_t count_of_shapes = 0;
      for (size_t j = 0; j < shapes.size(); ++j)
      {
        if (shapes[j]->isInside({x, y}) && !count_of_shapes)
        {
          results.first += 1;
          count_of_shapes = 1;
        }
        else if (shapes[j]->isInside({x, y}))
        {
          count_of_shapes += 1;
        }
      }
      if (count_of_shapes == shapes.size())
      {
        results.second += 1;
      }
    }
    return results;
  }
  double getMaxYAmongShapes(const shape_t &shapes)
  {
    double result = shapes[0]->getMaxY();
    for (size_t i = 1; i < shapes.size(); ++i)
    {
      result = std::max(result, shapes[i]->getMaxY());
    }
    return result;
  }
  double getMinYAmongShapes(const shape_t &shapes)
  {
    double result = shapes[0]->getMinY();
    for (size_t i = 1; i < shapes.size(); ++i)
    {
      result = std::min(result, shapes[i]->getMinY());
    }
    return result;
  }
  double getMaxXAmongShapes(const shape_t &shapes)
  {
    double result = shapes[0]->getMaxX();
    for (size_t i = 1; i < shapes.size(); ++i)
    {
      result = std::max(result, shapes[i]->getMaxX());
    }
    return result;
  }
  double getMinXAmongShapes(const shape_t &shapes)
  {
    double result = shapes[0]->getMinX();
    for (size_t i = 1; i < shapes.size(); ++i)
    {
      result = std::min(result, shapes[i]->getMinX());
    }
    return result;
  }
}

namespace muh = muhamadiarov;

std::pair< double, double > muh::getAreas(
    const muh::shape_t &shapes, const size_t threads, const size_t tries, const size_t seed)
{
  std::pair< double, double > results = {0.0, 0.0};
  const size_t count_of_tries_for_thread = tries / (threads + 1);
  const size_t remaining = tries % (threads + 1);
  std::vector< std::future< std::pair< size_t, size_t > > > futures;
  const double y_max = muh::getMaxYAmongShapes(shapes);
  const double y_min = muh::getMinYAmongShapes(shapes);
  double x_max = muh::getMaxXAmongShapes(shapes);
  double x_min = muh::getMinXAmongShapes(shapes);
  const double step = (x_max - x_min) / (threads + 1);
  x_max = x_min + step;
  for (size_t i = 1; i < threads + 1; ++i)
  {
    x_min = x_max;
    x_max += step;
    futures.push_back(std::async(std::launch::async, muh::getCountOfPoints, std::ref(shapes), count_of_tries_for_thread,
        seed, x_max, x_min, y_max, y_min));
  }
  x_min = muh::getMinXAmongShapes(shapes);
  x_max = x_min + step;
  std::pair< size_t, size_t > count
      = muh::getCountOfPoints(shapes, count_of_tries_for_thread + remaining, seed, x_max, x_min, y_max, y_min);
  for (size_t i = 0; i < threads; ++i)
  {
    const std::pair< size_t, size_t > r = futures[i].get();
    count.first += r.first;
    count.second += r.second;
  }
  x_max = muh::getMaxXAmongShapes(shapes);
  const double box_area = (y_max - y_min) * (x_max - x_min);
  results.first = box_area * static_cast< double >(count.first) / tries;
  results.second = box_area * static_cast< double >(count.second) / tries;
  return results;
}
