#include "area.hpp"
#include <random>
#include <cstddef>
#include <future>
#include <utility>
#include <limits>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <functional>
#include "shapes.hpp"

namespace dirko {
  namespace {
    struct borders_t {
      dirko::point_t max, min;
    };
    std::pair< size_t, size_t > calculate(const shapes_t &shapes, point_t max, point_t min, size_t tests, size_t seed)
    {
      std::default_random_engine gen(seed);
      std::uniform_real_distribution< double > dist_x(min.x, max.x);
      std::uniform_real_distribution< double > dist_y(min.y, max.y);
      size_t intersection_count = 0;
      size_t combination_count = 0;
      for (size_t i = 0; i < tests; i++) {
        bool is_inside_any = false;
        bool is_inside_all = true;
        const point_t p{dist_x(gen), dist_y(gen)};
        for (const auto &shape : shapes) {
          if (shape->isInside(p)) {
            is_inside_any = true;
          } else {
            is_inside_all = false;
          }
        }
        if (is_inside_all) {
          intersection_count++;
        }
        if (is_inside_any) {
          combination_count++;
        }
      }
      return {intersection_count, combination_count};
    }
    borders_t getBorders(const shapes_t &shapes)
    {
      const double inf = std::numeric_limits< double >::infinity();
      point_t max{-inf, -inf};
      point_t min{inf, inf};
      for (const auto &shape : shapes) {
        const point_t shapeMax = shape->getMaxCorner();
        const point_t shapeMin = shape->getMinCorner();
        max.x = std::max(max.x, shapeMax.x);
        max.y = std::max(max.y, shapeMax.y);
        min.x = std::min(min.x, shapeMin.x);
        min.y = std::min(min.y, shapeMin.y);
      }
      return {max, min};
    }
  }
  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed)
  {
    if (!tests) {
      throw std::invalid_argument("zero tests");
    }
    if (shapes.empty()) {
      return {0.0, 0.0};
    }
    if (threads == 0) {
      threads = 1;
    }
    const size_t maxThreads = 12;
    if (threads > maxThreads) {
      threads = maxThreads;
    }
    std::vector< std::future< std::pair< size_t, size_t > > > futures;
    const size_t testsPerThread = tests / threads;
    const size_t remainder = tests % threads;
    const borders_t border = getBorders(shapes);
    for (size_t i = 0; i < threads; i++) {
      const size_t part = i < remainder ? testsPerThread + 1 : testsPerThread;
      futures.push_back(
          std::async(std::launch::async, calculate, std::cref(shapes), border.max, border.min, part, seed + i));
    }
    size_t inters = 0;
    size_t covers = 0;
    for (auto &future : futures) {
      const std::pair< size_t, size_t > result = future.get();
      inters += result.first;
      covers += result.second;
    }
    const double borderArea = (border.max.x - border.min.x) * (border.max.y - border.min.y);
    return {borderArea * (static_cast< double >(covers) / tests), borderArea * (static_cast< double >(inters) / tests)};
  }
}
