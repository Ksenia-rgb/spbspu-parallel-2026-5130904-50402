#include "worker.hpp"
#include <chrono>
#include <cstddef>
#include <ratio>
#include <unistd.h>
#include "../common/area.hpp"
#include "../common/shape.hpp"

namespace karpovich
{

  size_t take(int &err, int rd, void *buf, size_t k)
  {
    char *const p = static_cast< char * >(buf);
    size_t r = 0;
    while (r < k)
    {
      err = read(rd, p + r, k - r);
      if (err <= 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  size_t send(int &err, int wr, const void *buf, size_t k)
  {
    const char *const p = static_cast< const char * >(buf);
    size_t r = 0;
    while (r < k)
    {
      err = write(wr, p + r, k - r);
      if (err <= 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  void runWorker(int rd, int result_wr, size_t seed)
  {
    while (true)
    {
      size_t hdr[4] = {};
      int err = 0;
      if (take(err, rd, hdr, sizeof(hdr)) != sizeof(hdr))
      {
        break;
      }

      shapes_t shapes(hdr[3]);

      if (!shapes.empty())
      {
        const size_t bytes = shapes.size() * sizeof(shape_t);

        if (take(err, rd, shapes.data(), bytes) != bytes)
        {
          break;
        }
      }

      const auto start = std::chrono::steady_clock::now();
      const double area_val = area(shapes, hdr[1], hdr[2], seed + hdr[0]).second;
      const auto end = std::chrono::steady_clock::now();
      const double ms = std::chrono::duration< double, std::milli >(end - start).count();
      const ResultSlot result{hdr[0], ms, area_val};

      if (send(err, result_wr, &result, sizeof(result)) != sizeof(result))
      {
        break;
      }
    }
  }
}
