#include "area.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <random>
#include <utility>
#include <vector>
#include "shape.hpp"

namespace {
  using hits_t = std::pair< std::size_t, std::size_t >;

  samarin::frame_t getFrame(const samarin::shapes_t & shapes)
  {
    samarin::frame_t frame = shapes.front()->getFrame();
    for (const std::unique_ptr< samarin::Shape > & shape : shapes) {
      const samarin::frame_t shapeFrame = shape->getFrame();
      frame.min.x = std::min(frame.min.x, shapeFrame.min.x);
      frame.min.y = std::min(frame.min.y, shapeFrame.min.y);
      frame.max.x = std::max(frame.max.x, shapeFrame.max.x);
      frame.max.y = std::max(frame.max.y, shapeFrame.max.y);
    }
    return frame;
  }

  hits_t countHits(const samarin::shapes_t & shapes, samarin::frame_t frame, std::size_t tries, std::size_t seed)
  {
    std::minstd_rand0 generator(seed);
    std::uniform_real_distribution< double > xDistribution(frame.min.x, frame.max.x);
    std::uniform_real_distribution< double > yDistribution(frame.min.y, frame.max.y);
    hits_t hits{0, 0};
    for (std::size_t i = 0; i < tries; ++i) {
      const samarin::point_t point{xDistribution(generator), yDistribution(generator)};
      const auto isInside = [point](const std::unique_ptr< samarin::Shape > & shape)
      {
        return shape->contains(point);
      };
      const std::size_t inside = std::count_if(shapes.cbegin(), shapes.cend(), isInside);
      if (inside > 0) {
        ++hits.first;
      }
      if (inside == shapes.size()) {
        ++hits.second;
      }
    }
    return hits;
  }
}

samarin::areas_t samarin::getAreas(const shapes_t & shapes, std::size_t threads, std::size_t tries, std::size_t seed)
{
  if (shapes.empty()) {
    return {0.0, 0.0};
  }
  const frame_t frame = getFrame(shapes);
  const std::size_t workers = std::max< std::size_t >(threads, 1);
  std::vector< std::future< hits_t > > futures;
  futures.reserve(workers);
  for (std::size_t i = 0; i < workers; ++i) {
    const std::size_t part = (tries / workers) + (i < tries % workers ? 1 : 0);
    futures.push_back(std::async(std::launch::async, countHits, std::cref(shapes), frame, part, seed + i));
  }
  hits_t total{0, 0};
  for (std::future< hits_t > & future : futures) {
    const hits_t hits = future.get();
    total.first += hits.first;
    total.second += hits.second;
  }
  const double width = frame.max.x - frame.min.x;
  const double height = frame.max.y - frame.min.y;
  const double frameArea = width * height;
  const double count = tries;
  return {frameArea * total.first / count, frameArea * total.second / count};
}
