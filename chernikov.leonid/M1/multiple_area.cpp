#include "multiple_area.hpp"
#include <functional>
#include <random>
#include <thread>

bool chernikov::isInside(double x, double y, const circle_t &circle)
{
  const double dx = x - circle.x;
  const double dy = y - circle.y;
  return dx * dx + dy * dy <= circle.r * circle.r;
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
  std::uniform_real_distribution< double > dist_x(c.x - c.r, c.x + c.r);
  std::uniform_real_distribution< double > dist_y(c.y - c.r, c.y + c.r);
  size_t hits = 0;
  for (size_t i = 0; i < data.tests; ++i) {
    const double x = dist_x(gen);
    const double y = dist_y(gen);
    if (isInside(x, y, c)) {
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