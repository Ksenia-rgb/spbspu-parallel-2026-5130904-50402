#include "monte_carlo.hpp"
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>
#include "figure.hpp"
#include "options.hpp"

namespace
{
  class ThreadJoiner final
  {
  public:
    ThreadJoiner(const ThreadJoiner&) = delete;
    explicit ThreadJoiner(std::vector< std::thread >& threads);
    ~ThreadJoiner();
    ThreadJoiner& operator=(const ThreadJoiner&) = delete;

  private:
    std::vector< std::thread >& threads_;
  };
}

ThreadJoiner::ThreadJoiner(std::vector< std::thread >& threads):
  threads_(threads)
{}

ThreadJoiner::~ThreadJoiner()
{
  for (std::thread& thread : threads_)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }
}

namespace
{
  struct hits_t
  {
    std::uint64_t coverage;
    std::uint64_t intersection;
  };

  static hits_t countHits(const std::vector< matveev::figure_t >& figures, const matveev::bounds_t& bounds,
      const std::uint64_t tries, const std::uint64_t seed)
  {
    std::default_random_engine generator(seed);
    std::uniform_real_distribution< double > x_distribution(
        static_cast< double >(bounds.min_x), static_cast< double >(bounds.max_x));
    std::uniform_real_distribution< double > y_distribution(
        static_cast< double >(bounds.min_y), static_cast< double >(bounds.max_y));
    hits_t hits{0, 0};
    for (std::uint64_t trial = 0; trial < tries; ++trial)
    {
      const long double x = x_distribution(generator);
      const long double y = y_distribution(generator);
      bool inside_any = false;
      bool inside_all = true;
      for (const matveev::figure_t& figure : figures)
      {
        const bool inside = matveev::isInside(figure, x, y);
        inside_any = inside_any || inside;
        inside_all = inside_all && inside;
      }
      if (inside_any)
      {
        ++hits.coverage;
      }
      if (inside_all)
      {
        ++hits.intersection;
      }
    }
    return hits;
  }

  static hits_t runTrials(const std::vector< matveev::figure_t >& figures, const matveev::bounds_t& bounds,
      const matveev::options_t& options)
  {
    if (options.threads == 0)
    {
      return countHits(figures, bounds, options.tries, options.seed);
    }
    std::vector< hits_t > results(options.threads, hits_t{0, 0});
    const std::uint64_t base_tries = options.tries / options.threads;
    const std::uint64_t remainder = options.tries % options.threads;
    {
      std::vector< std::thread > workers{};
      workers.reserve(options.threads);
      const ThreadJoiner joiner(workers);
      for (std::size_t index = 0; index < options.threads; ++index)
      {
        const std::uint64_t count = base_tries + (index < remainder);
        const std::uint64_t seed = options.seed + static_cast< std::uint64_t >(index);
        workers.emplace_back(
            [&figures, &bounds, &results, index, count, seed]()
            {
              results[index] = countHits(figures, bounds, count, seed);
            });
      }
    }
    hits_t total{0, 0};
    for (const hits_t& result : results)
    {
      total.coverage += result.coverage;
      total.intersection += result.intersection;
    }
    return total;
  }
}

matveev::areas_t matveev::calculateAreas(const std::vector< figure_t >& figures, const options_t& options)
{
  if (options.tries == 0)
  {
    throw std::invalid_argument("The number of trials must be positive");
  }
  if (figures.empty())
  {
    return {0.0L, 0.0L};
  }
  const bounds_t bounds = findBounds(figures);
  const hits_t hits = runTrials(figures, bounds, options);
  const long double rectangle_area = (bounds.max_x - bounds.min_x) * (bounds.max_y - bounds.min_y);
  const long double trials = static_cast< long double >(options.tries);
  return {rectangle_area * (static_cast< long double >(hits.coverage) / trials),
      rectangle_area * (static_cast< long double >(hits.intersection) / trials)};
}
