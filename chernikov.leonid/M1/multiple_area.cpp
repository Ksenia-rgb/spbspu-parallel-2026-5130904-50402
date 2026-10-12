#include "multiple_area.hpp"
#include <algorithm>
#include <functional>
#include <random>
#include <thread>

bool chernikov::isInside(const point_t &p, const circle_t &circle)
{
  const double dx = p.x - circle.center.x;
  const double dy = p.y - circle.center.y;
  return dx * dx + dy * dy <= circle.r * circle.r;
}

chernikov::frame_t chernikov::getShapesFrame(const circles_t &circles)
{
  const circle_t &first = circles.front();
  frame_t frame = {
    {first.center.x - first.r, first.center.y - first.r},
    {first.center.x + first.r, first.center.y + first.r}
  };
  for (const circle_t &c: circles) {
  const point_t circle_min = {c.center.x - c.r, c.center.y - c.r};
  const point_t circle_max = {c.center.x + c.r, c.center.y + c.r};
  frame.min.x = std::min(frame.min.x, circle_min.x);
  frame.min.y = std::min(frame.min.y, circle_min.y);
  frame.max.x = std::max(frame.max.x, circle_max.x);
  frame.max.y = std::max(frame.max.y, circle_max.y);
  }
  return frame;
}

void chernikov::fillThreadData(std::vector< thread_data_t > &data, const circles_t &circles, const frame_t &frame, size_t tests, size_t seed)
{
  const size_t threads = data.size();
  const size_t whole = tests / threads;
  const size_t rem = tests % threads;
  for (size_t i = 0; i < threads; ++i) {
    data[i].circles = &circles;
    data[i].frame = frame;
    data[i].tests = (i < rem) ? whole + 1 : whole;
    data[i].seed = seed + i;
    data[i].in_any = 0;
    data[i].in_all = 0;
  }
}

void chernikov::countHits(thread_data_t &data)
{
  size_t in_any = 0;
  size_t in_all = 0;
  std::default_random_engine gen(data.seed);
  std::uniform_real_distribution< double > dist_x(data.frame.min.x, data.frame.max.x);
  std::uniform_real_distribution< double > dist_y(data.frame.min.y, data.frame.max.y);
  for (size_t i = 0; i < data.tests; ++i) {
    const point_t p = {dist_x(gen), dist_y(gen)};
    bool any = false;
    bool all = true;
    for (const circle_t &c: *data.circles) {
      if (isInside(p, c)) {
        any = true;
      } else {
        all = false;
      }
    }
    if (any) {
      ++in_any;
    }
    if (all) {
      ++in_all;
    }
  }
  data.in_any = in_any;
  data.in_all = in_all;
}

std::pair< double, double > chernikov::area(const circles_t &circles, size_t threads, size_t tests, size_t seed)
{
  if (circles.empty()) {
    return {0.0, 0.0};
  }
  if (threads == 0) {
    threads = 1;
  }
  const frame_t frame = getShapesFrame(circles);
  std::vector< thread_data_t > data(threads);
  fillThreadData(data, circles, frame, tests, seed);
  std::vector< std::thread > workers;
  workers.reserve(threads);
  for (size_t i = 0; i < threads; ++i) {
    workers.emplace_back(countHits, std::ref(data[i]));
  }
  size_t in_any = 0;
  size_t in_all = 0;
  for (size_t i = 0; i < threads; ++i) {
    workers[i].join();
    in_any += data[i].in_any;
    in_all += data[i].in_all;
  }
  const double frame_area = (frame.max.x - frame.min.x) * (frame.max.y - frame.min.y);
  const double total = static_cast< double >(tests);
  return {frame_area * static_cast< double >(in_any) / total, frame_area * static_cast< double >(in_all) / total};
}
