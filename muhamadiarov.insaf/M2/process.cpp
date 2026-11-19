#include "process.hpp"
#include "../common/monte_carlo.hpp"
#include "../common/shapes.hpp"
#include <unistd.h>
#include <chrono>
#include <util>
#include <cstddef>
#include <memory>
#include <vector>

namespace muhamadiarov
{

  std::size_t send(int& err, int wr, const void* buffer, std::size_t k) const noexcept
  {
    char* const p = static_cast< char* >(buffer);
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

  std::size_t take(int& err, int rd, void* buffer, std::size_t k) const noexcept
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

  void childLoop(int wr, int rd)
  {
    signal(SIGPIPE, SIG_IGN);
    while (true)
    {
      Task task;
      int err = 0;
      if (take(err, rd, &task, sizeof(Task)) != sizeof(Task) || err != 0)
      {
        break;
      }

      std::vector< TaskShape > raw(task.countShapes);
      if (task.countShapes > 0)
      {
        const std::size_t bytes = sizeof(TaskShape) * task.countShapes;
        if(take(err, rd, raw.data(), bytes) != bytes || err != 0)
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

      const auto start = std::chrono::steady_clock::now();
      const std::pair< double, double > areas = getAreas(shapes, task.threads, task.tries, task.seed);
      const auto end = std::chrono::steady_clock::now();
      const double ms = std::chrono::duration< double, std::milli >(end - start).count();
      
      Result res;
      res.area = areas.first;
      res.time = ms;
      if (send(err, wr, &res, sizeof(Result)) != sizeof(Result) || err != 0)
      {
        break;
      }

      close(rd);
      close(out);
      _exit(0);
    }
  }

}
