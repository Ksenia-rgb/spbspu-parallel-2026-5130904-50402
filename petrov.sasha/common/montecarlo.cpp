#include "montecarlo.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>
#include <thread>
#include <vector>

namespace petrov {

  namespace {

    void countWorkerHits(
        const std::vector< shape_t >& shapes, const frame_t& frame, std::size_t tries, std::size_t seed, hits_t& result)
    {
      std::default_random_engine generator(seed);
      const double generator_begin = static_cast< double >(generator.min());
      const double generator_width = static_cast< double >(generator.max()) - generator_begin;
      const double step_x = (frame.max_point.x - frame.min_point.x) / generator_width;
      const double step_y = (frame.max_point.y - frame.min_point.y) / generator_width;
      hits_t hits = {};
      for (std::size_t attempt = 0; attempt < tries; ++attempt) {
        const double x = frame.min_point.x + ((static_cast< double >(generator()) - generator_begin) * step_x);
        const double y = frame.min_point.y + ((static_cast< double >(generator()) - generator_begin) * step_y);
        const point_t point = {x, y};
        bool inside_union = false;
        bool inside_intersection = true;
        for (const shape_t& shape : shapes) {
          const bool inside = isInside(point, shape);
          inside_union = inside_union || inside;
          inside_intersection = inside_intersection && inside;
        }
        if (inside_union) {
          ++hits.union_count;
        }
        if (inside_intersection) {
          ++hits.intersection_count;
        }
      }
      result = hits;
    }

  }

  hits_t countHits(const std::vector< shape_t >& shapes, const frame_t& frame, std::size_t tries, std::size_t threads,
      std::size_t seed)
  {
    const std::size_t requested_count = (threads == 0) ? 1 : threads;
    const std::size_t hardware_count = static_cast< std::size_t >(std::thread::hardware_concurrency());
    const std::size_t hardware_limit = (hardware_count == 0) ? 1 : hardware_count;
    const std::size_t worker_count = std::min(requested_count, hardware_limit);
    const std::size_t tries_per_worker = tries / worker_count;
    const std::size_t remainder = tries % worker_count;

    std::vector< hits_t > results(worker_count);
    std::vector< std::thread > workers;
    workers.reserve(worker_count);
    for (std::size_t worker_index = 0; worker_index < worker_count; ++worker_index) {
      const std::size_t worker_tries = tries_per_worker + ((worker_index < remainder) ? 1 : 0);
      workers.emplace_back(countWorkerHits, std::cref(shapes), std::cref(frame), worker_tries, seed + worker_index,
          std::ref(results[worker_index]));
    }
    for (std::thread& worker : workers) {
      worker.join();
    }

    hits_t total = {};
    for (const hits_t& hits : results) {
      total.union_count += hits.union_count;
      total.intersection_count += hits.intersection_count;
    }
    return total;
  }

  double computeArea(const frame_t& frame, std::size_t hits, std::size_t tries)
  {
    const double hit_share = static_cast< double >(hits) / static_cast< double >(tries);
    return frameArea(frame) * hit_share;
  }

  double coveredArea(const std::vector< shape_t >& shapes, std::size_t threads, std::size_t tries, std::size_t seed)
  {
    if (shapes.empty() || (tries == 0)) {
      return 0.0;
    }
    const frame_t frame = findFrame(shapes);
    const hits_t hits = countHits(shapes, frame, tries, threads, seed);
    return computeArea(frame, hits.union_count, tries);
  }

}
