#include "area.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <future>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "shape.hpp"

namespace karpovich
{
  namespace
  {
    struct Box
    {
      Point max;
      Point min;
    };

    std::pair< size_t, size_t > calculate(const shapes_t &shapes, Point max, Point min, size_t tests, size_t seed)
    {
      std::default_random_engine gen(seed);
      std::uniform_real_distribution< double > dist_x(min.x, max.x);
      std::uniform_real_distribution< double > dist_y(min.y, max.y);
      size_t hits_intersection = 0;
      size_t hits_cover = 0;
      for (size_t i = 0; i < tests; i++)
      {
        bool is_inside_any = false;
        bool is_inside_all = true;
        const Point p{dist_x(gen), dist_y(gen)};
        for (const auto &shp : shapes)
        {
          if (shp->contains(p))
          {
            is_inside_any = true;
          }
          else
          {
            is_inside_all = false;
          }
        }
        if (is_inside_all)
        {
          hits_intersection++;
        }
        if (is_inside_any)
        {
          hits_cover++;
        }
      }
      return {hits_intersection, hits_cover};
    }

    Box findBox(const shapes_t &shapes)
    {
      const double inf = std::numeric_limits< double >::infinity();
      Point max{-inf, -inf};
      Point min{inf, inf};
      for (const auto &shape : shapes)
      {
        const Point shape_min = shape->getMinCorner();
        const Point shape_max = shape->getMaxCorner();
        min.x = std::min(min.x, shape_min.x);
        min.y = std::min(min.y, shape_min.y);
        max.x = std::max(max.x, shape_max.x);
        max.y = std::max(max.y, shape_max.y);
      }
      return {max, min};
    }

  }

  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed)
  {
    if (!tests)
    {
      throw std::invalid_argument("tests must be > 0");
    }
    if (shapes.empty())
    {
      return {0.0, 0.0};
    }
    if (threads == 0)
    {
      threads = 1;
    }
    constexpr size_t max_threads = 12;
    if (threads > max_threads)
    {
      threads = max_threads;
    }
    std::vector< std::future< std::pair< size_t, size_t > > > futures;
    const size_t tests_per_thread = tests / threads;
    const size_t remainder = tests % threads;
    const Box box = findBox(shapes);
    for (size_t i = 0; i < threads; i++)
    {
      const size_t part = i < remainder ? tests_per_thread + 1 : tests_per_thread;
      futures.push_back(std::async(std::launch::async, calculate, std::cref(shapes), box.max, box.min, part, seed + i));
    }
    size_t inters = 0;
    size_t covers = 0;
    for (auto &future : futures)
    {
      const std::pair< size_t, size_t > result = future.get();
      inters += result.first;
      covers += result.second;
    }
    const double box_area = (box.max.x - box.min.x) * (box.max.y - box.min.y);
    return {box_area * (static_cast< double >(covers) / tests), box_area * (static_cast< double >(inters) / tests)};
  }

}
