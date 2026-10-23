#include "worker.hpp"
#include <chrono>
#include <cstddef>
#include <memory>
#include <ratio>
#include <unistd.h>
#include <vector>
#include "../common/area.hpp"
#include "../common/shape.hpp"

namespace karpovich
{
  namespace
  {

    std::unique_ptr< karpovich::Shape > makeShape(const karpovich::ShapeData &d)
    {
      using namespace karpovich;
      if (d.type == ShapeType::Circle)
      {
        return std::make_unique< Circle >(d.r1, point_t{d.x, d.y});
      }
      return std::make_unique< Ellipse >(d.r1, d.r2, point_t{d.x, d.y});
    }

  }

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
      size_t hdr[PROTO_HEADER_WORDS] = {};
      int err = 0;
      take(err, rd, hdr, sizeof hdr);
      if (err <= 0)
      {
        break;
      }

      std::vector< ShapeData > raw(hdr[4]);
      if (!raw.empty())
      {
        err = 0;
        take(err, rd, raw.data(), raw.size() * sizeof(ShapeData));
        if (err <= 0)
        {
          break;
        }
      }

      shapes_t shapes;
      shapes.reserve(raw.size());
      for (const auto &d : raw)
      {
        shapes.push_back(makeShape(d));
      }

      const auto start = std::chrono::steady_clock::now();
      const double area_val = area(shapes, hdr[2], hdr[3], seed + hdr[1]).second;
      const auto end = std::chrono::steady_clock::now();
      const double ms = std::chrono::duration< double, std::milli >(end - start).count();
      const ResultSlot result{hdr[1], ms, area_val};
      err = 0;
      send(err, result_wr, &result, sizeof result);
      if (err <= 0)
      {
        break;
      }
    }
  }
}
