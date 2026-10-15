#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <memory>
#include <vector>

namespace samarin {
  struct point_t {
    double x;
    double y;
  };

  struct frame_t {
    point_t min;
    point_t max;
  };

  class Shape {
  public:
    virtual ~Shape() = default;
    virtual bool contains(point_t point) const noexcept = 0;
    virtual frame_t getFrame() const noexcept = 0;
  };

  using shapes_t = std::vector< std::unique_ptr< Shape > >;
}

#endif
