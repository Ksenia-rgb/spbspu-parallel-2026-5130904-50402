#ifndef CIRCLE_HPP
#define CIRCLE_HPP

namespace novikov
{
  class Circle
  {
  public:
    Circle(double r, double x, double y);
    double getR() const;
    double getX() const;
    double getY() const;

  private:
    double r_, x_, y_;
  };
}

#endif
