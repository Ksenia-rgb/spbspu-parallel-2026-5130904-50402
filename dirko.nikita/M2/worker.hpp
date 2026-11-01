#ifndef WORKER_HPP
#define WORKER_HPP
#include <cstddef>
#include <sys/types.h>

namespace dirko {

  struct Result {
    size_t calc_id;
    double ms;
    double area;
  };

  struct Worker {
    pid_t pid = -1;
    int input = -1;
    int output = -1;
  };

  void runWorker(int input, int output, size_t seed);

  size_t take(int &err, int rd, void *buf, size_t k);
  size_t send(int &err, int wr, const void *buf, size_t k);

}

#endif
