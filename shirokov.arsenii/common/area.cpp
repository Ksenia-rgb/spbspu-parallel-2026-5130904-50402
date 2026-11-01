#include "area.hpp"
#include <algorithm>
#include <functional>
#include <future>
#include <random>
#include <vector>

std::pair< size_t, size_t > shirokov::calc(
    const ellipse_list& ellipses, size_t tries, size_t seed, const MinMaxes& min_maxes)
{
  std::default_random_engine engine(seed);
  const bool use_fast_gen = (ellipses.size() == 1 || tries >= 10000000);

  std::uniform_real_distribution< double > dist_x(min_maxes.min_x, min_maxes.max_x);
  std::uniform_real_distribution< double > dist_y(min_maxes.min_y, min_maxes.max_y);

  const double eng_min = static_cast< double >(std::default_random_engine::min());
  const double eng_range = static_cast< double >(std::default_random_engine::max() - std::default_random_engine::min());
  const double scale_x = (min_maxes.max_x - min_maxes.min_x) / eng_range;
  const double scale_y = (min_maxes.max_y - min_maxes.min_y) / eng_range;

  size_t count_of_total_coverage = 0;
  size_t count_of_intersection = 0;

  for (size_t i = 0; i < tries; ++i)
  {
    double x = 0.0;
    double y = 0.0;
    if (use_fast_gen)
    {
      x = min_maxes.min_x + (static_cast< double >(engine()) - eng_min) * scale_x;
      y = min_maxes.min_y + (static_cast< double >(engine()) - eng_min) * scale_y;
    }
    else
    {
      x = dist_x(engine);
      y = dist_y(engine);
    }

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
      if (is_in_circle && !is_in_intersection)
      {
        break;
      }
    }
    count_of_total_coverage += is_in_circle;
    count_of_intersection += is_in_intersection;
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

  const size_t base = tries / threads;
  const size_t remainder = tries % threads;
  const MinMaxes min_maxes = shirokov::getAllMinMaxes(ellipses);

  std::vector< std::future< std::pair< size_t, size_t > > > results;
  results.reserve(threads - 1);

  for (size_t i = 0; i + 1 < threads; ++i)
  {
    const size_t curr_tries_count = base + ((i < remainder) ? 1 : 0);
    results.push_back(std::async(std::launch::async, calc, std::cref(ellipses), curr_tries_count, seed + i, min_maxes));
  }

  const size_t last_idx = threads - 1;
  const size_t last_tries = base + ((last_idx < remainder) ? 1 : 0);
  const std::pair< size_t, size_t > last_res = calc(ellipses, last_tries, seed + last_idx, min_maxes);

  size_t count_of_total_coverage = last_res.first;
  size_t count_of_intersection = last_res.second;
  for (size_t i = 0; i + 1 < threads; ++i)
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
