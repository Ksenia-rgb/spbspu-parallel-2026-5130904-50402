#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "../common/shapes.hpp"
#include <cstddef>
#include <string>
#include <map>
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
    Task(int thread, int tries, std::size_t seed, std::size_t countShapes);
    int thread = 0;
    int tries = 0;
    std::size_t seed = 0;
    std::size_t countShapes = 0; 
  };

  struct TaskShape
  {
    double r = 0;
    double x = 0;
    double y = 0;
  };

  struct InfoProcess
  {
    InfoProcess(const std::string& name, std::size_t seed, pid_t pid = -1, int writeFd = -1, int readFd = -1) = default;
    std::string name;
    pid_t pid = -1;
    StateProcess state = StateProcess::STAND;
    int writeFd = -1;
    int readFd = -1;
    std::size_t seed = 0;
  };

  std::size_t send(int& err, int rd, const void* buffer, std::size_t k);
  std::size_t take(int& err, int wr, void* buffer, std::size_t k);
  
  void childLoop(int wr, int rd, Task task);

}

#endif
