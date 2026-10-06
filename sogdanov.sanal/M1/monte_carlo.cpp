#include "monte_carlo.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <thread>

namespace sogdanov
{
  namespace
  {
    struct HitCount
    {
      std::size_t coverage = 0;
      std::size_t intersection = 0;
    };

    Bounds findGeneralBounds(const std::vector< Figure >& figures)
    {
      Bounds bounds = figures.front().getBounds();

      for (const Figure& figure : figures)
      {
        const Bounds current = figure.getBounds();

        bounds.min_x = std::min(bounds.min_x, current.min_x);
        bounds.max_x = std::max(bounds.max_x, current.max_x);
        bounds.min_y = std::min(bounds.min_y, current.min_y);
        bounds.max_y = std::max(bounds.max_y, current.max_y);
      }

      return bounds;
    }

    HitCount countHits(
        const std::vector< Figure >& figures, const Bounds& bounds, const std::size_t tries, const std::size_t seed)
    {
      std::mt19937_64 generator(seed);
      std::uniform_real_distribution< long double > coordinate_x(bounds.min_x, bounds.max_x);
      std::uniform_real_distribution< long double > coordinate_y(bounds.min_y, bounds.max_y);
      HitCount hits{};

      for (std::size_t index = 0; index < tries; ++index)
      {
        const long double x = coordinate_x(generator);
        const long double y = coordinate_y(generator);
        bool in_coverage = false;
        bool in_intersection = true;

        for (const Figure& figure : figures)
        {
          const bool inside = figure.contains(x, y);

          in_coverage = in_coverage || inside;
          in_intersection = in_intersection && inside;
        }

        hits.coverage += static_cast< std::size_t >(in_coverage);
        hits.intersection += static_cast< std::size_t >(in_intersection);
      }

      return hits;
    }
  }
}

sogdanov::Areas sogdanov::estimateAreas(
    const std::vector< Figure >& figures, std::size_t thread_count, const std::size_t tries, const std::size_t seed)
{
  if (tries == 0)
  {
    throw std::invalid_argument("The number of tries must be positive");
  }
  if (figures.empty())
  {
    return {0.0, 0.0};
  }
  if (thread_count == 0)
  {
    thread_count = std::max< std::size_t >(1, std::thread::hardware_concurrency());
  }

  const Bounds bounds = findGeneralBounds(figures);
  const std::size_t base_tries = tries / thread_count;
  const std::size_t remainder = tries % thread_count;
  std::vector< HitCount > results(thread_count);
  std::vector< std::thread > workers{};

  workers.reserve(thread_count);
  try
  {
    for (std::size_t i = 0; i < thread_count; ++i)
    {
      const std::size_t local_tries = base_tries + (i < remainder ? 1 : 0);
      const std::size_t local_seed = seed + i;

      workers.emplace_back(
          [&, i, local_tries, local_seed]()
          {
            results[i] = countHits(figures, bounds, local_tries, local_seed);
          });
    }
  }
  catch (...)
  {
    for (std::thread& worker : workers)
    {
      worker.join();
    }

    throw;
  }

  for (std::thread& worker : workers)
  {
    worker.join();
  }

  HitCount total{};

  for (const HitCount& result : results)
  {
    total.coverage += result.coverage;
    total.intersection += result.intersection;
  }

  const long double rectangle_area = (bounds.max_x - bounds.min_x) * (bounds.max_y - bounds.min_y);
  const long double coverage_ratio = static_cast< long double >(total.coverage) / tries;
  const long double intersection_ratio = static_cast< long double >(total.intersection) / tries;

  return {rectangle_area * coverage_ratio, rectangle_area * intersection_ratio};
}
