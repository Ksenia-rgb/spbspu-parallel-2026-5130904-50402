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
    DONE
  };

  struct ResultProcess
  {
    std::string processName;
    double time = 0.0;
    double area = 0.0;
  };

  class InfoProcess
  {
  public:
    InfoProcess();
    InfoProcess(const InfoProcess&) = delete;
    InfoProcess& operator=(const InfoProcess&) = delete;
    InfoProcess(InfoProcess&& other) noexcept;
    InfoProcess& operator=(InfoProcess&& other) noexcept;
    InfoProcess(pid_t pid, int writeFd, int readFd);

    std::size_t send(int& err, const void* buffer, std::size_t k) const noexcept;
    std::size_t take(int& err, void* buffer, std::size_t k) const noexcept;
    
    ResultProcess result() const;
    StateProcess state() const noexcept;
    pid_t pid() const noexcept;
    
    void run() const;
    void wait() const;
    
    ~InfoProcess();
  
  private:
    ResultProcess result_;
    pid_t pid_ = -1;
    StateProcess state_ = StateProcess::STAND;
    int writeFd_ = -1;
    int readFd_ = -1;
  };

}

#endif
