#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "Point.hpp"
#include "Shapes.hpp"

namespace khalikov
{
  struct Circle : khalikov::Shapes
  {
    khalikov::Point center;
    double r;

    bool isIn(const Point &pt) const override
    {
      double dx = pt.x - center.x;
      double dy = pt.y - center.y;
      return dx * dx + dy * dy <= r * r;
    }

    double minX() const override
    {
      return center.x - r;
    }


    double minY() const override
    {
      return center.y - r;
    }


    double maxX() const override
    {
      return center.x + r;
    }


    double maxY() const override
    {
      return center.y + r;
    }

  };
}

#endif
