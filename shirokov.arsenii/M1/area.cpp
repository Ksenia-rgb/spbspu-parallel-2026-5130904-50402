#include "area.hpp"
#include <future>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

std::pair< size_t, size_t > shirokov::calc(
    const ellipse_list& ellipses, size_t tries, size_t seed, const MinMaxes& min_maxes)
{
  std::default_random_engine engine(seed);

  std::uniform_real_distribution< double > dist_x(min_maxes.min_x, min_maxes.max_x);
  std::uniform_real_distribution< double > dist_y(min_maxes.min_y, min_maxes.max_y);

  size_t count_of_total_coverage = 0;
  size_t count_of_intersection = 0;
  for (size_t i = 0; i < tries; ++i)
  {
    const double x = dist_x(engine);
    const double y = dist_y(engine);
    bool is_in_circle = false;
    bool is_in_intersection = true;
    for (const Ellipse& s : ellipses)
    {
      if (s.isInside(x, y))
      {
        is_in_circle = true;
      }
      else
      {
        is_in_intersection = false;
      }
    }
    if (is_in_circle)
    {
      ++count_of_total_coverage;
    }
    if (is_in_intersection)
    {
      ++count_of_intersection;
    }
  }
  return {count_of_total_coverage, count_of_intersection};
}

const std::pair< double, double > shirokov::area(
    const ellipse_list& ellipses, size_t threads, size_t tries, size_t seed)
{
  if (!threads)
  {
    threads = 1;
  }

  constexpr size_t max_allowed_threads = 64;
  threads = std::min(threads, max_allowed_threads);
  threads = std::min(threads, tries);

  std::vector< std::future< std::pair< size_t, size_t > > > results;
  results.reserve(threads);

  const size_t base = tries / threads;
  const size_t remainder = tries % threads;

  const MinMaxes min_maxes = shirokov::getAllMinMaxes(ellipses);

  for (size_t i = 0; i < threads; ++i)
  {
    const size_t curr_tries_count = base + ((i < remainder) ? 1 : 0);
    results.push_back(std::async(std::launch::async, calc, std::cref(ellipses), curr_tries_count, seed + i, min_maxes));
  }

  size_t count_of_total_coverage = 0;
  size_t count_of_intersection = 0;
  for (size_t i = 0; i < threads; ++i)
  {
    const std::pair< size_t, size_t > res = results[i].get();
    count_of_total_coverage += res.first;
    count_of_intersection += res.second;
  }

  const double box_area = (min_maxes.max_x - min_maxes.min_x) * (min_maxes.max_y - min_maxes.min_y);

  const double total_coverage
      = (static_cast< double >(count_of_total_coverage) / static_cast< double >(tries)) * box_area;
  const double intersection_area
      = (static_cast< double >(count_of_intersection) / static_cast< double >(tries)) * box_area;

  return {total_coverage, intersection_area};
}

