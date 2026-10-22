#include "worker.hpp"
#include <cstddef>
#include <memory>
#include <unistd.h>
#include <vector>
#include <chrono>
#include "../common/shape.hpp"
#include "../common/area.hpp"

namespace karpovich
{

  namespace
  {
    size_t take(int &err, int rd, void *buf, size_t k)
    {
      char *p = static_cast< char * >(buf);
      size_t r = 0;
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

  void runWorker(int rd, size_t seed, WorkerShared *sh)
  {
    while (true)
    {
      size_t hdr[5] = {};
      int err = 0;
      take(err, rd, hdr, sizeof hdr);
      if (err < 0)
      {
        break;
      }

      std::vector< ShapeData > raw(hdr[4]);
      err = 0;
      take(err, rd, raw.data(), raw.size() * sizeof(ShapeData));
      if (err < 0)
      {
        break;
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

      pthread_mutex_lock(&sh->mtx);
      const int idx = sh->completed;
      if (idx < WORKER_MAX_SLOTS)
      {
        sh->slots[idx].calc_id = hdr[1];
        sh->slots[idx].ms = ms;
        sh->slots[idx].area = area_val;
        sh->completed = idx + 1;
      }
      pthread_mutex_unlock(&sh->mtx);
    }
  }

}
