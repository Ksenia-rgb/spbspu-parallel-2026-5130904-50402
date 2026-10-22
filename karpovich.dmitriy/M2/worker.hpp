#ifndef WORKER_HPP
#define WORKER_HPP

#include <sys/types.h>

namespace karpovich
{
  struct Worker
  {
    pid_t pid = -1;
    int wr = -1;
  };

  void runWorker(int rd, size_t seed);
}

#endif
