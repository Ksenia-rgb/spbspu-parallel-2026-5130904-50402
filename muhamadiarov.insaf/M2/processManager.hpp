#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP

#include <cstddef>
#include <string>
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
    InfoProcess(pid_t pid, int write, int read);

    std::size_t send(int& err, const void* buffer, std::size_t k) const noexcept;
    std::size_t take(int& err, void* buffer, std::size_t k) const noexcept;
    
    ResultProcess result() const;
    StateProcess state() const noexcept;
    pid_t pid() const noexcept;
    
    void run() const noexcept;
    void wait() const noexcept;
    
    ~InfoProcess();
  
  private:
    ResultProcess result_;
    pid_t pid_ = -1;
    StateProcess state_ = StateProcess::STAND;
    int write_ = -1;
    int read_ = -1;
  }; 

}

#endif
