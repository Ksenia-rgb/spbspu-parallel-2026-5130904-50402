#include "multiple_area.hpp"
#include <functional>
#include <random>
#include <thread>

bool chernikov::isInside(double x, double y, double r)
{
  return x * x + y * y <= r * r;
}

void chernikov::fillThreadData(std::vector< thread_data_t > &data, double r, size_t tests)
{
  const size_t threads = data.size();
  const size_t whole = tests / threads;
  const size_t rem = tests % threads;
  for (size_t i = 0; i < threads; ++i) {
    data[i].r = r;
    data[i].tests = (i < rem) ? whole + 1 : whole;
    data[i].seed = i;
    data[i].result = 0;
  }
}

void chernikov::countHits(thread_data_t &data)
{
  std::mt19937 gen(data.seed);
  std::uniform_real_distribution< double > dist(-data.r, data.r);
  size_t hits = 0;
  for (size_t i = 0; i < data.tests; ++i) {
    const double x = dist(gen);
    const double y = dist(gen);
    if (isInside(x, y, data.r)) {
      ++hits;
    }
  }
  data.result = hits;
}

double chernikov::area(double r, size_t threads, size_t tests)
{
  std::vector< thread_data_t > data(threads);
  fillThreadData(data, r, tests);

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

  const double square_area = (2 * r) * (2 * r);
  return square_area * static_cast< double >(hits) / static_cast< double >(tests);
}