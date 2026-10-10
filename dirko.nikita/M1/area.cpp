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

std::pair< size_t, size_t > dirko::calculate(
    const circles_t &circles, point_t min, point_t max, size_t tests, size_t seed)
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
    for (const Circle &circle : circles) {
      if (circle.isInside(p)) {
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
std::pair< dirko::point_t, dirko::point_t > dirko::getBorders(const circles_t &circles)
{
  const double inf = std::numeric_limits< double >::infinity();
  point_t max{-inf, -inf};
  point_t min{inf, inf};
  for (const Circle &circle : circles) {
    const point_t circleMax = {circle.position.x + circle.radius, circle.position.y + circle.radius};
    const point_t circleMin = {circle.position.x - circle.radius, circle.position.y - circle.radius};
    max.x = std::max(max.x, circleMax.x);
    max.y = std::max(max.y, circleMax.y);
    min.x = std::min(min.x, circleMin.x);
    min.y = std::min(min.y, circleMin.y);
  }
  return {max, min};
}
std::pair< double, double > dirko::area(const circles_t &circles, size_t threads, size_t tests, size_t seed)
{
  if (!tests) {
    throw std::invalid_argument("zero tests");
  }
  if (circles.empty()) {
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
  const std::pair< point_t, point_t > border = getBorders(circles);
  for (size_t i = 0; i < threads; i++) {
    const size_t part = i < remainder ? testsPerThread + 1 : testsPerThread;
    futures.push_back(
        std::async(std::launch::async, calculate, std::cref(circles), border.second, border.first, part, seed + i));
  }
  size_t inters = 0;
  size_t covers = 0;
  for (auto &future : futures) {
    const std::pair< size_t, size_t > result = future.get();
    inters += result.first;
    covers += result.second;
  }
  const double borderArea = (border.first.x - border.second.x) * (border.first.y - border.second.y);
  return {borderArea * (static_cast< double >(covers) / tests), borderArea * (static_cast< double >(inters) / tests)};
}
