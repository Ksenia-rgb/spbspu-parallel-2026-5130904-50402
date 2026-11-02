#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include "shape.hpp"

namespace samarin {
  class Ellipse final: public Shape {
  public:
    Ellipse(double horizontalRadius, double verticalRadius, point_t center);
    bool contains(point_t point) const noexcept override;
    frame_t getFrame() const noexcept override;

  private:
    double horizontalRadius_;
    double verticalRadius_;
    point_t center_;
  };
}

#endif
