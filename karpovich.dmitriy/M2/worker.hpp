#ifndef WORKER_HPP
#define WORKER_HPP
#include <cstddef>
#include <sys/types.h>

namespace karpovich
{
  constexpr size_t PROTO_HEADER_WORDS = 5;

  enum class ShapeType : unsigned char
  {
    Circle = 0,
    Ellipse = 1,
  };

  struct ShapeData
  {
    ShapeType type;
    double r1;
    double r2;
    double x;
    double y;
  };

  struct ResultSlot
  {
    size_t calc_id;
    double ms;
    double area;
  };

  struct Worker
  {
    pid_t pid = -1;
    int wr = -1;
    int result_rd = -1;
  };

  void runWorker(int rd, int result_wr, size_t seed);

  size_t take(int &err, int rd, void *buf, size_t k);
  size_t send(int &err, int wr, const void *buf, size_t k);

}

#endif
