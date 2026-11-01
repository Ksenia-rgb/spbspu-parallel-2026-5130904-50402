#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>
#include <queue>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <memory>
#include "calcmap.hpp"
#include "circle.hpp"

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

    void start(std::vector<std::shared_ptr<Circle>> &circles, std::string calc_name, size_t threads, size_t tries);

    double get_result();
    void wait_for(const std::string& calc_name, calc_map& global_cm);
    void poll_results(calc_map& global_cm);

    void set_seed(int seed);
    bool is_alive() const;

    bool at(const std::string& calc_name) const;
    const CalcRes& get_calc(const std::string& calc_name) const;

  private:
    void cleanup();

    std::string process_name_;
    std::queue<std::string> pending_calcs_;
    int seed_;

    calc_map calc_results_;

    pid_t pid_;
    int in_fd_;
    int out_fd_;
  };
}

#endif
