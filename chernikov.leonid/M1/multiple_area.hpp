#ifndef MULTIPLE_AREA_HPP
#define MULTIPLE_AREA_HPP

#include <cstddef>
#include <vector>

namespace chernikov {
  struct thread_data_t {
    double r;
    size_t tests;
    size_t seed;
    size_t result;
  };

  bool isInside(double x, double y, double r);
  void fillThreadData(std::vector< thread_data_t > &data, double r, size_t tests);
  void countHits(thread_data_t &data);
  double area(double r, size_t threads, size_t tests);
}

#endif