#include "process.hpp"
#include "../common/monte_carlo.hpp"
#include "../common/shapes.hpp"
#include <unistd.h>
#include <chrono>
#include <utility>
#include <cstddef>
#include <memory>
#include <vector>
#include <csignal>

namespace muhamadiarov
{

  namespace
  {

    constexpr int max_tries = 5'000'000;

  }

  std::size_t send(int& err, int wr, const void* buffer, std::size_t k)
  {
    const char* const p = static_cast< const char* >(buffer);
    std::size_t r = 0;
    while (r < k)
    {
      err = write(wr, p + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  std::size_t take(int& err, int rd, void* buffer, std::size_t k)
  {
    char* const p = static_cast< char* >(buffer);
    std::size_t r = 0;
    while (r < k)
    {
      err = read(rd, p + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  void childLoop(int rd, int wr)
  {
    signal(SIGPIPE, SIG_IGN);
    while (true)
    {
      Task task{};
      int err = 0;
      if (take(err, rd, &task, sizeof(Task)) != sizeof(Task))
      {
        break;
      }

      std::vector< TaskShape > raw(task.count_shapes);
      if (task.count_shapes > 0)
      {
        const std::size_t bytes = sizeof(TaskShape) * task.count_shapes;
        if (take(err, rd, raw.data(), bytes) != bytes)
        {
          break;
        }
      }

      shape_t shapes;
      shapes.reserve(raw.size());
      for (const TaskShape& ts : raw)
      {
        shapes.push_back(std::make_unique< Circle >(ts.r, Point{ts.x, ts.y}));
      }

      const int effective_tries = std::min(task.tries, max_tries);
      const auto start = std::chrono::steady_clock::now();
      const std::pair< double, double > areas = getAreas(shapes, task.threads, effective_tries, task.seed);
      const auto end = std::chrono::steady_clock::now();
      const double ms = std::chrono::duration< double, std::milli >(end - start).count();

      Result res;
      res.area = areas.first;
      res.time = ms;
      if (send(err, wr, &res, sizeof(Result)) != sizeof(Result))
      {
        break;
      }
    }

    close(rd);
    close(wr);
    _exit(0);
  }

}
