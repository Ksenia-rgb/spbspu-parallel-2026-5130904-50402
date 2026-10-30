#include "monte_carlo.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>

#include <cmath>
#include <limits>

namespace sogdanov
{
  namespace
  {
    struct HitCount
    {
      size_t coverage = 0;
      size_t intersection = 0;
    };

    static HitCount countHits(
        const std::vector< Figure > &figures, const Bounds &bounds, const size_t tries, const size_t seed)
    {
      struct prepared_figure_t
      {
        double center_x;
        double center_y;
        double inverse_radius_x;
        double inverse_radius_y;
      };

      HitCount hits{};

      if (tries == 0)
      {
        return hits;
      }

      const long double width = bounds.max_x - bounds.min_x;
      const long double height = bounds.max_y - bounds.min_y;
      std::vector< prepared_figure_t > prepared;

      prepared.reserve(figures.size());

      for (const Figure &figure : figures)
      {
        const long double center_x = figure.getCenterX();
        const long double center_y = figure.getCenterY();

        prepared.push_back({static_cast< double >((center_x - bounds.min_x) / width),
            static_cast< double >((center_y - bounds.min_y) / height),
            static_cast< double >(width / figure.getHorizontalRadius()),
            static_cast< double >(height / figure.getVerticalRadius())});
      }

      std::mt19937_64 generator(seed);

      const int precision = std::numeric_limits< double >::digits;
      const size_t shift = std::mt19937_64::word_size - static_cast< size_t >(precision);
      const double scale = std::ldexp(1.0, -precision);

      const prepared_figure_t *const begin = prepared.data();
      const prepared_figure_t *const end = begin + prepared.size();

      for (size_t index = 0; index < tries; ++index)
      {
        const double x = static_cast< double >(generator() >> shift) * scale;
        const double y = static_cast< double >(generator() >> shift) * scale;

        bool in_coverage = false;
        bool in_intersection = true;

        for (const prepared_figure_t *figure = begin; figure != end; ++figure)
        {
          const double horizontal = (x - figure->center_x) * figure->inverse_radius_x;
          const double vertical = (y - figure->center_y) * figure->inverse_radius_y;
          const bool inside = (horizontal * horizontal + vertical * vertical) <= 1.0;

          in_coverage = in_coverage || inside;
          in_intersection = in_intersection && inside;

          if (in_coverage && !in_intersection)
          {
            break;
          }
        }

        hits.coverage += static_cast< size_t >(in_coverage);
        hits.intersection += static_cast< size_t >(in_intersection);
      }

      return hits;
    }

    static void joinThreads(std::vector< std::thread > &threads)
    {
      for (std::thread &thread : threads)
      {
        if (thread.joinable())
        {
          thread.join();
        }
      }
    }
  }
}

sogdanov::Bounds sogdanov::findGeneralBounds(const std::vector< Figure > &figures)
{
  if (figures.empty())
  {
    throw std::invalid_argument("An empty set has no bounding rectangle");
  }

  Bounds bounds = figures.front().getBounds();

  for (const Figure &figure : figures)
  {
    const Bounds current = figure.getBounds();

    bounds.min_x = std::min(bounds.min_x, current.min_x);
    bounds.max_x = std::max(bounds.max_x, current.max_x);
    bounds.min_y = std::min(bounds.min_y, current.min_y);
    bounds.max_y = std::max(bounds.max_y, current.max_y);
  }

  return bounds;
}

sogdanov::Areas sogdanov::estimateAreas(
    const std::vector< Figure > &figures, size_t thread_count, const size_t tries, const size_t seed)
{
  if (tries == 0)
  {
    throw std::invalid_argument("The number of tries must be positive");
  }

  if (figures.empty())
  {
    return {0.0L, 0.0L};
  }

  if (thread_count == 0)
  {
    thread_count = std::max< size_t >(1, std::thread::hardware_concurrency());
  }

  const Bounds bounds = findGeneralBounds(figures);
  const size_t base_tries = tries / thread_count;
  const size_t remainder = tries % thread_count;
  std::vector< HitCount > results(thread_count);
  std::vector< std::thread > workers{};

  workers.reserve(thread_count);

  try
  {
    for (size_t index = 0; index < thread_count; ++index)
    {
      const size_t local_tries = base_tries + ((index < remainder) ? 1 : 0);
      const size_t local_seed = seed + index;

      workers.emplace_back(
          [&figures, &bounds, &results, index, local_tries, local_seed]()
          {
            results[index] = countHits(figures, bounds, local_tries, local_seed);
          });
    }
  }
  catch (...)
  {
    joinThreads(workers);
    throw;
  }

  joinThreads(workers);

  HitCount total{};

  for (const HitCount &result : results)
  {
    total.coverage += result.coverage;
    total.intersection += result.intersection;
  }

  const long double width = bounds.max_x - bounds.min_x;
  const long double height = bounds.max_y - bounds.min_y;
  const long double rectangle_area = width * height;
  const long double coverage_ratio = static_cast< long double >(total.coverage) / tries;
  const long double intersection_ratio = static_cast< long double >(total.intersection) / tries;

  return {rectangle_area * coverage_ratio, rectangle_area * intersection_ratio};
}
