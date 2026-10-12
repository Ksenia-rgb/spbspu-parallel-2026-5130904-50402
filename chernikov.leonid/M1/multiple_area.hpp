#ifndef MULTIPLE_AREA_HPP
#define MULTIPLE_AREA_HPP

#include <cstddef>
#include <vector>

namespace chernikov {
  struct point_t {
    double x;
    double y;
  };
  struct circle_t {
    double r;
    point_t center;
  };
  struct frame_t {
    point_t min;
    point_t max;
  };
  struct thread_data_t {
    circle_t circle;
    size_t tests;
    size_t seed;
    size_t result;
  };

  bool isInside(const point_t &p, const circle_t &circle);
  //frame_t getShapesFrame();
  void fillThreadData(std::vector< thread_data_t > &data, const circle_t &circle, size_t tests);
  void countHits(thread_data_t &data);
  double area(const circle_t &circle, size_t threads, size_t tests);
}

#endif