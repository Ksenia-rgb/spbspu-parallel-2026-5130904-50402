#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <circle.hpp>
#include "calcmap.hpp"

namespace lavrentev
{
  class Process
  {
  public:
    Process(const Process &) = delete;
    Process(Process &&other) noexcept;
    explicit Process(const std::string &process_name);
    ~Process();

    Process &operator=(const Process &) = delete;
    Process &operator=(Process &&other) noexcept;

    void start(
      std::vector< std::shared_ptr< Circle > > &circles,
      const std::string &calc_name,
      std::size_t threads,
      std::size_t tries
    );
    double getResult();
    void waitFor(const std::string &calc_name, calc_map &global_cm);
    void pollResults(calc_map &global_cm);
    void setSeed(int seed);
    bool isAlive() const;
    bool at(const std::string &calc_name) const;
    const CalcRes &getCalc(const std::string &calc_name) const;

  private:
    std::string process_name_;
    std::queue< std::string > pending_calcs_;
    int seed_;
    calc_map calc_results_;
    pid_t pid_;
    int in_fd_;
    int out_fd_;

    void cleanup();
  };
}

#endif
