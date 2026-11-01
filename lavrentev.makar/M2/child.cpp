#include "child.hpp"

#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <string>
#include <vector>

#include "calculations.hpp"

namespace
{
  const double MIN_TIME_MS = 1.0;

  struct CircleData
  {
    int r;
    int x;
    int y;
  };

  class Clicker
  {
  public:
    Clicker():
      start_(std::chrono::high_resolution_clock::now())
    {
    }

    double getMillisec() const
    {
      using std::chrono::duration_cast;
      using std::chrono::high_resolution_clock;
      using std::chrono::milliseconds;

      const auto current_time = high_resolution_clock::now();
      return static_cast< double >(duration_cast< milliseconds >(current_time - start_).count());
    }

  private:
    std::chrono::high_resolution_clock::time_point start_;
  };

  bool readAll(int fd, void *data, std::size_t size)
  {
    char *ptr = static_cast< char * >(data);
    while (size > 0)
    {
      const ssize_t res = read(fd, ptr, size);
      if (res <= 0)
      {
        return false;
      }
      ptr += res;
      size -= static_cast< std::size_t >(res);
    }
    return true;
  }

  bool writeAll(int fd, const void *data, std::size_t size)
  {
    const char *ptr = static_cast< const char * >(data);
    while (size > 0)
    {
      const ssize_t res = write(fd, ptr, size);
      if (res <= 0)
      {
        return false;
      }
      ptr += res;
      size -= static_cast< std::size_t >(res);
    }
    return true;
  }
}

int lavrentev::runChild(int in_fd, int out_fd)
{
  while (true)
  {
    std::size_t str_len = 0;
    if (!readAll(in_fd, &str_len, sizeof(str_len)))
    {
      break;
    }

    std::string calc_name(str_len, '\0');
    if ((str_len > 0) && !readAll(in_fd, &calc_name[0], str_len))
    {
      break;
    }

    std::size_t threads = 0;
    if (!readAll(in_fd, &threads, sizeof(threads)))
    {
      break;
    }

    std::size_t tries = 0;
    if (!readAll(in_fd, &tries, sizeof(tries)))
    {
      break;
    }

    int seed = 0;
    if (!readAll(in_fd, &seed, sizeof(seed)))
    {
      break;
    }

    std::size_t circles_count = 0;
    if (!readAll(in_fd, &circles_count, sizeof(circles_count)))
    {
      break;
    }

    lavrentev::f_t figures;
    figures.reserve(circles_count);
    if (circles_count > 0)
    {
      std::vector< CircleData > raw_data(circles_count);
      const std::size_t raw_size = circles_count * sizeof(CircleData);
      if (!readAll(in_fd, raw_data.data(), raw_size))
      {
        break;
      }
      for (const auto &item : raw_data)
      {
        figures.emplace_back(item.r, item.x, item.y);
      }
    }

    const lavrentev::Polygon pg = lavrentev::buildBoundingBox(figures);
    Clicker cl;
    const double init = cl.getMillisec();
    const lavrentev::area_pair_t res = lavrentev::area(figures, pg, threads, tries, seed);
    const double total = cl.getMillisec();

    const double result = res.first;
    double time = total - init;
    if (time < MIN_TIME_MS)
    {
      time = MIN_TIME_MS;
    }

    if (!writeAll(out_fd, &result, sizeof(result)))
    {
      break;
    }
    if (!writeAll(out_fd, &time, sizeof(time)))
    {
      break;
    }
  }

  close(in_fd);
  close(out_fd);
  return 0;
}
