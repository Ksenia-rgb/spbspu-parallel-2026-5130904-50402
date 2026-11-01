#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <chrono>
#include "calculations.hpp"

namespace
{
  struct CircleData
  {
    int r;
    int x;
    int y;
  };

  bool read_all(int fd, void* data, size_t size)
  {
    char* ptr = static_cast<char*>(data);
    while (size > 0)
    {
      ssize_t res = read(fd, ptr, size);
      if (res <= 0) return false;
      ptr += res;
      size -= res;
    }
    return true;
  }

  bool write_all(int fd, const void* data, size_t size)
  {
    const char* ptr = static_cast<const char*>(data);
    while (size > 0)
    {
      ssize_t res = write(fd, ptr, size);
      if (res <= 0) return false;
      ptr += res;
      size -= res;
    }
    return true;
  }

  class Clicker {
  public:
    Clicker():
      start_(std::chrono::high_resolution_clock::now()) {}
    double millisec() const {
      using std::chrono::high_resolution_clock;
      using std::chrono::duration_cast;
      using std::chrono::milliseconds;
      auto t = high_resolution_clock::now();
      return duration_cast<milliseconds>(t - start_).count();
    }
  private:
    std::chrono::high_resolution_clock::time_point start_;
  };
}

int run_child(int in_fd, int out_fd)
{
  while (true)
  {
    size_t str_len = 0;
    if (!read_all(in_fd, &str_len, sizeof(str_len)))
    {
      break;
    }

    std::string calc_name(str_len, '\0');
    if (str_len > 0 && !read_all(in_fd, &calc_name[0], str_len))
    {
      break;
    }

    size_t threads = 0;
    size_t tries = 0;
    int seed = 0;
    if (!read_all(in_fd, &threads, sizeof(threads)) ||
      !read_all(in_fd, &tries, sizeof(tries)) ||
      !read_all(in_fd, &seed, sizeof(seed)))
    {
      break;
    }

    size_t circles_count = 0;
    if (!read_all(in_fd, &circles_count, sizeof(circles_count)))
    {
      break;
    }

    lavrentev::f_t figures;
    figures.reserve(circles_count);

    if (circles_count > 0)
    {
      std::vector<CircleData> raw_data(circles_count);
      if (!read_all(in_fd, raw_data.data(), circles_count * sizeof(CircleData)))
      {
        break;
      }

      for (const auto& item : raw_data)
      {
        figures.emplace_back(item.r, item.x, item.y);
      }
    }

    lavrentev::Polygon pg = lavrentev::buildBoundingBox(figures);
    Clicker cl;
    double init = cl.millisec();
    lavrentev::area_pair_t res = lavrentev::area(figures, pg, threads, tries, seed);
    double total = cl.millisec();
    double result = res.first;
    double time = total - init;
    if (time < 1.0)
    {
      time = 1.0;
    }

    if (!write_all(out_fd, &result, sizeof(result)))
    {
      break;
    }
    if (!write_all(out_fd, &time, sizeof(time)))
    {
      break;
    }
  }

  close(in_fd);
  close(out_fd);
  return 0;
}
