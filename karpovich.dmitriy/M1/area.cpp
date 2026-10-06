#include "area.hpp"
#include <algorithm>
#include <future>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace karpovich
{

  struct Box
  {
    Point max;
    Point min;
  };

  std::pair< size_t, size_t > calculate(const std::vector< std::unique_ptr< Shape > > &shapes, Point max, Point min,
                                        size_t tests, size_t seed)
  {
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > distX(min.x, max.x);
    std::uniform_real_distribution< double > distY(min.y, max.y);
    size_t hitsIntersection = 0;
    size_t hitsCover = 0;
    for (size_t i = 0; i < tests; i++) {
      bool isInsideAny = false;
      bool isInsideAll = true;
      Point p{distX(gen), distY(gen)};
      for (const auto &shp : shapes) {
        if (shp->contains(p)) {
          isInsideAny = true;
        } else {
          isInsideAll = false;
        }
      }
      if (isInsideAll) {
        hitsIntersection++;
      }
      if (isInsideAny) {
        hitsCover++;
      }
    }
    return {hitsIntersection, hitsCover};
  }

  Box findBox(const std::vector< std::unique_ptr< Shape > > &shapes)
  {
    double inf = std::numeric_limits< double >::infinity();
    Point max{-inf, -inf};
    Point min{inf, inf};
    for (const auto &shape : shapes) {
      Point shapeMin = shape->getMinCorner();
      Point shapeMax = shape->getMaxCorner();
      min.x = std::min(min.x, shapeMin.x);
      min.y = std::min(min.y, shapeMin.y);
      max.x = std::max(max.x, shapeMax.x);
      max.y = std::max(max.y, shapeMax.y);
    }
    return {max, min};
  }

  AreaResult area(const std::vector< std::unique_ptr< Shape > > &shapes, size_t threads, size_t tests, size_t seed)
  {
    if (!threads || !tests) {
      throw std::invalid_argument("args must be > 0");
    }
    if (shapes.empty()) {
      return {0.0, 0.0};
    }
    std::vector< std::future< std::pair< size_t, size_t > > > futures;
    size_t tests_per_thread = tests / threads;
    size_t remainder = tests % threads;
    Box box = findBox(shapes);
    for (size_t i = 0; i < threads; i++) {
      size_t test_for_task = i < remainder ? tests_per_thread + 1 : tests_per_thread;
      futures.push_back(
          std::async(std::launch::async, calculate, std::cref(shapes), box.max, box.min, test_for_task, seed + i));
    }
    size_t inters = 0;
    size_t covers = 0;
    for (auto &future : futures) {
      std::pair< size_t, size_t > result = future.get();
      inters += result.first;
      covers += result.second;
    }
    double box_area = (box.max.x - box.min.x) * (box.max.y - box.min.y);
    return {box_area * (static_cast< double >(covers) / tests), box_area * (static_cast< double >(inters) / tests)};
  }

}
