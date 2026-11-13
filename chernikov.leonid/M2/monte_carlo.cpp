#include "monte_carlo.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <random>
#include <thread>
#include <vector>
#include "shapes.hpp"

chernikov::frame_t chernikov::getShapesFrame(const shapes_t &shapes)
{
  frame_t frame = shapes.front()->getFrame();
  for (const std::unique_ptr< Shape > &s : shapes) {
    frame = unite(frame, s->getFrame());
  }
  return frame;
}

void chernikov::fillThreadData(
    std::vector< thread_data_t > &data, const shapes_t &shapes, const frame_t &frame, size_t tests, size_t seed)
{
  const size_t threads = data.size();
  const size_t whole = tests / threads;
  const size_t rem = tests % threads;
  for (size_t i = 0; i < threads; ++i) {
    data[i].shapes = &shapes;
    data[i].frame = frame;
    data[i].tests = (i < rem) ? whole + 1 : whole;
    data[i].seed = seed + i;
    data[i].in_any = 0;
  }
}

void chernikov::countHits(thread_data_t &data)
{
  size_t in_any = 0;
  std::default_random_engine gen(data.seed);
  std::uniform_real_distribution< double > dist_x(data.frame.min.x, data.frame.max.x);
  std::uniform_real_distribution< double > dist_y(data.frame.min.y, data.frame.max.y);
  for (size_t i = 0; i < data.tests; ++i) {
    const point_t p = {dist_x(gen), dist_y(gen)};
    for (const std::unique_ptr< Shape > &s : *data.shapes) {
      if (s->contains(p)) {
        ++in_any;
        break;
      }
    }
  }
  data.in_any = in_any;
}

double chernikov::coverArea(const shapes_t &shapes, size_t threads, size_t tests, size_t seed)
{
  if (shapes.empty()) {
    return 0.0;
  }
  if (threads == 0) {
    threads = 1;
  }
  const frame_t frame = getShapesFrame(shapes);
  std::vector< thread_data_t > data(threads);
  fillThreadData(data, shapes, frame, tests, seed);
  std::vector< std::thread > workers;
  workers.reserve(threads);
  for (size_t i = 0; i < threads; ++i) {
    workers.emplace_back(countHits, std::ref(data[i]));
  }
  size_t in_any = 0;
  for (size_t i = 0; i < threads; ++i) {
    workers[i].join();
    in_any += data[i].in_any;
  }
  const double frame_area = (frame.max.x - frame.min.x) * (frame.max.y - frame.min.y);
  return frame_area * static_cast< double >(in_any) / static_cast< double >(tests);
}