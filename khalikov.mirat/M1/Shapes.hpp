#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "Point.hpp"

namespace khalikov
{
  struct Shapes
  {
    virtual ~Shapes() = default;

    virtual bool isIn(const Point &pt) const = 0;

    virtual double minX() const = 0;
    virtual double minY() const = 0;
    virtual double maxX() const = 0;
    virtual double maxY() const = 0;
    khalikov::Point center;
    double r;
  };
}

#endif
