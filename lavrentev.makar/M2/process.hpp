#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>

namespace lavrentev
{
  class Process
  {
  public:
    Process(std::string process_name);

    double get_result() const;
    void set_seed(int seed);

  private:
    std::string process_name_, calc_name_;
    int seed_;
    double result_;
  };
}

#endif
