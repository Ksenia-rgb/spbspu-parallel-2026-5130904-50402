#ifndef WORKER_HPP
#define WORKER_HPP

#include <cstddef>
#include <sys/types.h>

namespace afanasev
{
  struct ResultSlot
  {
    std::size_t calc_id = 0;
    double ms = 0.0;
    double area = 0.0;
  };

  struct RequestHeader
  {
    std::size_t calc_id = 0;
    std::size_t threads = 1;
    std::size_t tries = 0;
    std::size_t num_shapes = 0;
  };

  struct ShapeWire
  {
    double radius = 0.0;
    double cx = 0.0;
    double cy = 0.0;
  };

  struct Worker
  {
    pid_t pid = -1;
    int req_wr = -1;
    int result_rd = -1;
  };

  Worker spawnWorker(unsigned seed);
  void stopWorker(Worker & w);
  bool sendAll(int fd, const void * buf, std::size_t n);
  bool recvAll(int fd, void * buf, std::size_t n);
  bool hasData(int fd);
}

#endif
