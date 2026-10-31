#ifndef WORKER_HPP
#define WORKER_HPP

#include <deque>
#include <stddef.h>
#include <vector>

#include <sys/types.h>

#include "figure.hpp"

namespace sogdanov
{
  struct Worker
  {
    pid_t pid = -1;
    int wr = -1;
    int result_rd = -1;
    std::deque< std::vector< unsigned char > > outgoing;
    size_t sent = 0;
    size_t received = 0;
    unsigned char reply[sizeof(size_t) + 2 * sizeof(double)] = {};
  };

  struct ResultSlot
  {
    size_t calculation_id;
    double milliseconds;
    double area;
  };

  std::vector< unsigned char > makeRequest(
      size_t calculation_id, const std::vector< Figure >& figures, size_t threads, size_t tries);

  void runWorker(int input, int output, size_t seed);
}

#endif
