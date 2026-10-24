#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include "Point.hpp"
#include "Shapes.hpp"

namespace khalikov
{
  struct Ellipse : khalikov::Shapes
  {
    khalikov::Point center;
    double a;
    double b;

    bool isIn(const Point &pt) const override
    {
      double dx = pt.x - center.x;
      double dy = pt.y - center.y;
      return dx * dx / (a * a) + dy * dy / (b * b) <= 1.0;
    }

    double minX() const override
    {
      return center.x - a;
    }


    double minY() const override
    {
      return center.y - b;
    }


    double maxX() const override
    {
      return center.x + a;
    }


    double maxY() const override
    {
      return center.y + b;
    }

  };
}

#endif
