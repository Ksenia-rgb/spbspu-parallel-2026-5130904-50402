#include "timer.hpp"

shirokov::Clicker::Clicker():
  start_(std::chrono::high_resolution_clock::now())
{}

double shirokov::Clicker::millisec() const
{
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;
  const auto t = high_resolution_clock::now();
  return static_cast< double >(duration_cast< milliseconds >(t - start_).count());
}

