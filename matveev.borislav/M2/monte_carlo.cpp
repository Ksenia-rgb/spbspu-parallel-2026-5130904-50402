#include "monte_carlo.hpp"
#include <random>
#include <future>
#include <thread>
#include <algorithm>
#include <functional>
#include <stdexcept>

std::uint64_t matveev::countHits(const std::vector< shape_t > &shapes, const frame_t &frame,
    const std::uint64_t tries, const std::uint64_t seed)
{
  std::mt19937_64 generator(seed);
  std::uniform_real_distribution< double > horizontal(frame.left, frame.right);
  std::uniform_real_distribution< double > vertical(frame.bottom, frame.top);
  std::uint64_t hits = 0;
  for (std::uint64_t i = 0; i < tries; ++i)
  {
    const double x = horizontal(generator);
    const double y = vertical(generator);
    for (const shape_t &shape: shapes)
    {
      if (contains(shape, x, y))
      {
        ++hits;
        break;
      }
    }
  }
  return hits;
}

double matveev::calculateArea(const std::vector< shape_t > &shapes, std::uint64_t threads,
    const std::uint64_t tries, const std::uint64_t seed)
{
  if (tries == 0)
  {
    throw std::invalid_argument("Zero tries");
  }
  const std::vector< shape_t > localShapes = translateShapes(shapes);
  const frame_t frame = getFrame(localShapes);
  if (threads == 0)
  {
    constexpr unsigned int minimumThreads = 1;
    threads = std::max(minimumThreads, std::thread::hardware_concurrency());
  }
  std::vector< std::future< std::uint64_t > > futures = {};
  std::mt19937_64 seeds(seed);
  for (std::uint64_t i = 0; i < threads; ++i)
  {
    const std::uint64_t count = tries / threads + (i < tries % threads);
    futures.push_back(std::async(std::launch::async, countHits, std::cref(localShapes),
        std::cref(frame), count, seeds()));
  }
  std::uint64_t hits = 0;
  for (std::future< std::uint64_t > &result: futures)
  {
    hits += result.get();
  }
  const double rectangle = (frame.right - frame.left) * (frame.top - frame.bottom);
  return rectangle * (static_cast< double >(hits) / tries);
}
