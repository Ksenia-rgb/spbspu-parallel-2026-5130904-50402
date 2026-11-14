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
  const size_t count = data.shapes->size();
  std::vector< const Shape * > shapes(count);
  for (size_t i = 0; i < count; ++i) {
    shapes[i] = (*data.shapes)[i].get();
  }
  const Shape *const *const first = shapes.data();
  std::minstd_rand gen(data.seed);
  const double scale = 1.0 / (static_cast< double >(std::minstd_rand::max()) + 1.0);
  const double width = (data.frame.max.x - data.frame.min.x) * scale;
  const double height = (data.frame.max.y - data.frame.min.y) * scale;
  size_t in_any = 0;
  for (size_t i = 0; i < data.tests; ++i) {
    const point_t p = {data.frame.min.x + width * static_cast< double >(gen()),
        data.frame.min.y + height * static_cast< double >(gen())};
    for (size_t j = 0; j < count; ++j) {
      if (first[j]->contains(p)) {
        ++in_any;
        break;
      }
    }
  }
  data.in_any = in_any;
}

double chernikov::coverArea(const shapes_t &shapes, size_t threads, size_t tests, size_t seed)
{
  if (shapes.empty() || tests == 0) {
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
