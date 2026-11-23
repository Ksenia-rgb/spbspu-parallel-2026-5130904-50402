#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "../common/shapes.hpp"
#include <cstddef>
#include <string>
#include <sys/types.h>

namespace muhamadiarov
{

  enum class StateProcess
  {
    STAND,
    ACTIVE,
    DONE,
    CANCELED
  };

  struct Result
  {
    double time = 0.0;
    double area = 0.0;
  };

  struct Task
  {
    int threads = 0;
    int tries = 0;
    std::size_t seed = 0;
    std::size_t count_shapes = 0;
  };

  struct TaskShape
  {
    double r = 0;
    double x = 0;
    double y = 0;
  };

  struct InfoProcess
  {
    std::string name;
    pid_t pid = -1;
    StateProcess state = StateProcess::STAND;
    int write_fd = -1;
    int read_fd = -1;
    std::size_t seed = 0;
  };

  std::size_t send(int& err, int rd, const void* buffer, std::size_t k);
  std::size_t take(int& err, int wr, void* buffer, std::size_t k);

  void childLoop(int rd, int wr);

}

#endif
