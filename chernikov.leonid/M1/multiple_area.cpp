#include "multiple_area.hpp"
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
}

void chernikov::fillThreadData(std::vector< thread_data_t > &data, const circle_t &circle, size_t tests)
{
  const size_t threads = data.size();
  const size_t whole = tests / threads;
  const size_t rem = tests % threads;
  for (size_t i = 0; i < threads; ++i) {
    data[i].circle = circle;
    data[i].tests = (i < rem) ? whole + 1 : whole;
    data[i].seed = i;
    data[i].result = 0;
  }
}

void chernikov::countHits(thread_data_t &data)
{
  circle_t &c = data.circle;
  std::default_random_engine gen(data.seed);
  std::uniform_real_distribution< double > dist_x(c.center.x - c.r, c.center.x + c.r);
  std::uniform_real_distribution< double > dist_y(c.center.y - c.r, c.center.y + c.r);
  size_t hits = 0;
  for (size_t i = 0; i < data.tests; ++i) {
    const double x = dist_x(gen);
    const double y = dist_y(gen);
    point_t p {x,y};
    if (isInside(p, c)) {
      ++hits;
    }
  }
  data.result = hits;
}

double chernikov::area(const circle_t &circle, size_t threads, size_t tests)
{
  std::vector< thread_data_t > data(threads);
  fillThreadData(data, circle, tests);

  std::vector< std::thread > workers;
  workers.reserve(threads);
  for (size_t i = 0; i < threads; ++i) {
    workers.emplace_back(countHits, std::ref(data[i]));
  }

  size_t hits = 0;
  for (size_t i = 0; i < threads; ++i) {
    workers[i].join();
    hits += data[i].result;
  }

  const double square_area = (2 * circle.r) * (2 * circle.r);
  return square_area * static_cast< double >(hits) / static_cast< double >(tests);
}