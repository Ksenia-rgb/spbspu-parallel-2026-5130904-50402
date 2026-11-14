#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include "shapes.hpp"

namespace chernikov {
  using shapes_t = std::vector< std::unique_ptr< Shape > >;

  struct thread_data_t {
    const shapes_t *shapes;
    frame_t frame;
    size_t tests;
    size_t seed;
    size_t in_any;
  };

  frame_t getShapesFrame(const shapes_t &shapes);
  void fillThreadData(
      std::vector< thread_data_t > &data, const shapes_t &shapes, const frame_t &frame, size_t tests, size_t seed);
  void countHits(thread_data_t &data);
  double coverArea(const shapes_t &shapes, size_t threads, size_t tests, size_t seed);
}
#endif
