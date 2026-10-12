#ifndef MULTIPLE_AREA_HPP
#define MULTIPLE_AREA_HPP

#include <utility>
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
  using circles_t = std::vector< circle_t >;
  struct frame_t {
    point_t min;
    point_t max;
  };
  struct thread_data_t {
    const circles_t *circles;
    frame_t frame;
    size_t tests;
    size_t seed;
    size_t in_any;
    size_t in_all;
  };

  bool isInside(const point_t &p, const circle_t &circle);
  frame_t getShapesFrame(const circles_t &circles);
  void fillThreadData(
      std::vector< thread_data_t > &data, const circles_t &circles, const frame_t &frame, size_t tests, size_t seed);
  void countHits(thread_data_t &data);
  std::pair< double, double > area(const circles_t &circles, size_t threads, size_t tests, size_t seed);
}
#endif
