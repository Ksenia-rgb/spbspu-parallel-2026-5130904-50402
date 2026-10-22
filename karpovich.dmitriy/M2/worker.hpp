#ifndef WORKER_HPP
#define WORKER_HPP

#include <cstddef>
#include <pthread.h>
#include <string>
#include <sys/types.h>

namespace karpovich
{
  constexpr int WORKER_MAX_SLOTS = 1024;

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

  struct WorkerShared
  {
    pthread_mutex_t mtx;
    int completed;
    ResultSlot slots[WORKER_MAX_SLOTS];
  };

  struct Worker
  {
    pid_t pid = -1;
    int wr = -1;
    int shm_fd = -1;
    std::string shm_name;
    WorkerShared *sh = nullptr;
    int seen = 0;
  };

  void runWorker(int rd, int seed, WorkerShared *sh);
}

#endif
