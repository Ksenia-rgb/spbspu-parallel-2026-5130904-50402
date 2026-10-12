#ifndef MULTIPLE_AREA_HPP
#define MULTIPLE_AREA_HPP

#include <cstddef>
#include <vector>

namespace chernikov {
  struct circle_t {
    double r;
    double x;
    double y;
  };
  struct thread_data_t {
    circle_t circle;
    size_t tests;
    size_t seed;
    size_t result;
  };

  bool isInside(double x, double y, const circle_t &circle);
  void fillThreadData(std::vector< thread_data_t > &data, const circle_t &circle, size_t tests);
  void countHits(thread_data_t &data);
  double area(const circle_t &circle, size_t threads, size_t tests);
}

#endif