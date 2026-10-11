#include "areas.hpp"
#include <cstddef>
#include <functional>
#include <future>
#include <vector>
#include "circle.hpp"
#include "geometry.hpp"
#include "monte-carlo.hpp"

makarov::Areas makarov::getAreas(
    const std::vector< Circle >& circles, std::size_t threads, std::size_t tries, std::size_t seed)
{
  if (circles.empty())
  {
    return Areas{0.0, 0.0};
  }
  const Box box = getBox(circles);
  const std::size_t workers = (threads == 0) ? 1 : threads;
  const std::size_t part = tries / workers;
  const std::size_t rest = tries % workers;
  std::vector< std::future< Hits > > tasks;
  tasks.reserve(workers);
  for (std::size_t i = 0; i < workers; ++i)
  {
    const std::size_t count = (i < rest) ? part + 1 : part;
    tasks.push_back(std::async(std::launch::async, countHits, std::cref(circles), std::cref(box), count, seed + i));
  }
  Hits total{0, 0};
  for (std::future< Hits >& task : tasks)
  {
    const Hits hits = task.get();
    total.any += hits.any;
    total.all += hits.all;
  }
  const double boxArea = getArea(box);
  const double count = static_cast< double >(tries);
  return Areas{boxArea * static_cast< double >(total.any) / count, boxArea * static_cast< double >(total.all) / count};
}
