#include "monte-carlo.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>
#include <thread>
#include <vector>
#include "arguments.hpp"
#include "shapes.hpp"

namespace
{
  struct hits_t
  {
    std::size_t covered;
    std::size_t intersected;
  };

  using engine_t = std::minstd_rand0;
  using pool_t = std::vector< std::thread >;

  void countHits(const pozdnyakov::ellipses_t& ellipses, const pozdnyakov::rectangle_t& frame, std::size_t tries,
      std::size_t seed, hits_t& hits)
  {
    engine_t engine(static_cast< engine_t::result_type >(seed));
    std::uniform_real_distribution< double > abscissa(frame.left_bottom.x, frame.right_top.x);
    std::uniform_real_distribution< double > ordinate(frame.left_bottom.y, frame.right_top.y);
    hits_t local{0, 0};
    for (std::size_t i = 0; i < tries; ++i)
    {
      const double x = abscissa(engine);
      const double y = ordinate(engine);
      const pozdnyakov::point_t point{x, y};
      std::size_t owners = 0;
      for (const pozdnyakov::ellipse_t& ellipse : ellipses)
      {
        if (pozdnyakov::isInside(ellipse, point))
        {
          ++owners;
        }
      }
      if (owners > 0)
      {
        ++local.covered;
      }
      if (owners == ellipses.size())
      {
        ++local.intersected;
      }
    }
    hits = local;
  }

  std::size_t getWorkers(std::size_t threads, std::size_t tries)
  {
    return std::min(std::max< std::size_t >(threads, 1), tries);
  }

  std::size_t getShare(std::size_t tries, std::size_t workers, std::size_t index)
  {
    const std::size_t extra = (index < (tries % workers)) ? 1 : 0;
    return tries / workers + extra;
  }

  void joinAll(pool_t& pool)
  {
    for (std::thread& worker : pool)
    {
      worker.join();
    }
  }
}

pozdnyakov::areas_t pozdnyakov::computeAreas(const ellipses_t& ellipses, const arguments_t& args)
{
  const std::size_t tries = args.tries;
  const std::size_t seed = args.seed;
  if (ellipses.empty())
  {
    return {0.0, 0.0};
  }
  const rectangle_t frame = getFrame(ellipses);
  const std::size_t workers = getWorkers(args.threads, tries);
  std::vector< hits_t > hits(workers, hits_t{0, 0});
  pool_t pool;
  pool.reserve(workers - 1);
  try
  {
    for (std::size_t i = 1; i < workers; ++i)
    {
      const std::size_t share = getShare(tries, workers, i);
      pool.emplace_back(countHits, std::cref(ellipses), std::cref(frame), share, seed + i, std::ref(hits[i]));
    }
    countHits(ellipses, frame, getShare(tries, workers, 0), seed, hits[0]);
  }
  catch (...)
  {
    joinAll(pool);
    throw;
  }
  joinAll(pool);
  hits_t total{0, 0};
  for (const hits_t& part : hits)
  {
    total.covered += part.covered;
    total.intersected += part.intersected;
  }
  const double trial = getArea(frame) / static_cast< double >(tries);
  return {trial * static_cast< double >(total.covered), trial * static_cast< double >(total.intersected)};
}
