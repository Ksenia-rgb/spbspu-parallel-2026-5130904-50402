#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>
#include <sys/types.h>

namespace lavrentev
{
  class Process
  {
  public:
    explicit Process(std::string process_name);
    ~Process();

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    Process(Process&& other) noexcept;
    Process& operator=(Process&& other) noexcept;

    double get_result();
    void set_seed(int seed);
    bool is_alive() const;

  private:
    void cleanup();

    std::string process_name_, calc_name_;
    int seed_;
    double result_;

    pid_t pid_;
    int in_fd_;
    int out_fd_;
  };
}

#endif
