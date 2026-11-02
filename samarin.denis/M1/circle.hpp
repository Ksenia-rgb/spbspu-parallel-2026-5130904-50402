#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "shape.hpp"

namespace samarin {
  class Circle final: public Shape {
  public:
    Circle(double radius, point_t center);
    bool contains(point_t point) const noexcept override;
    frame_t getFrame() const noexcept override;

  private:
    double radius_;
    point_t center_;
  };
}

#endif
