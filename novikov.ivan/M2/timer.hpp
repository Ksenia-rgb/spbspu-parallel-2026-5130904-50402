#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace novikov
{
  class Clicker
  {
  public:
    Clicker();
    double millisec() const;

  private:
    std::chrono::high_resolution_clock::time_point start_;
  };
}

#endif
