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
    pid_t pid_ = -1;
    int wr_ = -1;
    int result_rd_ = -1;
    std::deque< std::vector< unsigned char > > outgoing_;
    size_t sent_ = 0;
    size_t received_ = 0;
    unsigned char reply_[sizeof(size_t) + 2 * sizeof(double)] = {};
  };

  struct ResultSlot
  {
    size_t calculation_id_;
    double milliseconds_;
    double area_;
  };

  std::vector< unsigned char > makeRequest(
      size_t calculation_id, const std::vector< Figure >& figures, size_t threads, size_t tries);

  void runWorker(int input, int output, size_t seed);
}

#endif
